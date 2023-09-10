/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: CAD performance evaluation test

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Benjamin Boulet
*/
#include <string.h>
#include <math.h>
#include "board.h"
#include "radio.h"

#if defined( USE_BAND_868 )

#define RF_FREQUENCY                                868000000 // Hz

#elif defined( USE_BAND_915 )

#define RF_FREQUENCY                                915000000 // Hz

#else
    #error "Please define a frequency band in the compiler options."
#endif

#define TX_OUTPUT_POWER                             14        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        108       // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         100       // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define LORA_FIX_LENGTH_PAYLOAD_LEN                 19

typedef enum
{
    LOWPOWER,
    RX,
    RX_TIMEOUT,
    RX_ERROR,
    TX,
    TX_TIMEOUT,
    START_CAD,
}States_t;

typedef enum
{
    CAD_FAIL,
    CAD_SUCCESS,
    PENDING,
}CadRx_t;

#define RX_TIMEOUT_VALUE                            4000
#define BUFFER_SIZE                                 64 // Define the payload size here

uint16_t BufferSize = BUFFER_SIZE;
uint8_t Buffer[BUFFER_SIZE];

States_t State = LOWPOWER;

int8_t RssiValue = 0;
int8_t SnrValue = 0;

/*!
 *	CAD performance evaluation's parameters
 */
#define RX_FW       1
#define TX_FW       0   //TX_FW is only for test
#define FULL_DBG    1   //Active all traces

#if(RX_FW == TX_FW)
    #error "Please define only one firmware."
#endif
CadRx_t CadRx = CAD_FAIL;
bool PacketReceived = false;
bool RxTimeoutTimerIrqFlag = false;
uint16_t channelActivityDetectedCnt = 0;
uint16_t RxCorrectCnt = 0;
uint16_t RxErrorCnt = 0;
uint16_t RxTimeoutCnt = 0;
uint16_t SymbTimeoutCnt = 0;
int16_t RssiMoy = 0;
int8_t SnrMoy = 0;

// Apps CAD timer
TimerEvent_t CADTimeoutTimer;
#define CAD_TIMER_TIMEOUT       1000        //Define de CAD timer's timeout here

TimerEvent_t RxAppTimeoutTimer;
#define RX_TIMER_TIMEOUT        4000        //Define de CAD timer's timeout here

//CAD parameters
#define CAD_SYMBOL_NUM          LORA_CAD_02_SYMBOL
#define CAD_DET_PEAK            22
#define CAD_DET_MIN             10
#define CAD_TIMEOUT_MS          2000
#define NB_TRY                  10

/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError( void );

/*!
 * \brief Function executed on Radio CAD Done event
 */
void OnCadDone( bool channelActivityDetected);

/*!
 * \brief Function configuring CAD parameters
 * \param [in]  cadSymbolNum   The number of symbol to use for CAD operations
 *                             [LORA_CAD_01_SYMBOL, LORA_CAD_02_SYMBOL,
 *                              LORA_CAD_04_SYMBOL, LORA_CAD_08_SYMBOL,
 *                              LORA_CAD_16_SYMBOL]
 * \param [in]  cadDetPeak     Limit for detection of SNR peak used in the CAD
 * \param [in]  cadDetMin      Set the minimum symbol recognition for CAD
 * \param [in]  cadTimeout     Defines the timeout value to abort the CAD activity
 */
void SX126xConfigureCad( RadioLoRaCadSymbols_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin , uint32_t cadTimeout);

/*!
 * \brief CAD timeout timer callback
 */
static void CADTimeoutTimeoutIrq( void );

/*!
 * \brief Rx timeout timer callback
 */
static void RxTimeoutTimerIrq( void );

/*!
 * \brief Average the collected RSSIs during CAD
 */
int8_t AverageCadRssi( void );

/*!
 * \brief Get the last good RSSI during CAD
 */
int8_t GetLastCadRssi( void );

/*!
 * \brief Display collected RSSIs each ms during CAD
 */
void DisplayCadRssivsTime( void );

/**
 * Main application entry point.
 */
int main( void )
{
    uint16_t PacketCnt = 0, i=0;
    float Per = 0.0;

    // Target board initialization
    BoardInitMcu( );
    BoardInitPeriph( );

    // Radio initialization
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    RadioEvents.CadDone = OnCadDone;
    
    //Timer used to restart the CAD
    TimerInit( &CADTimeoutTimer, CADTimeoutTimeoutIrq );
    TimerSetValue( &CADTimeoutTimer, CAD_TIMER_TIMEOUT );
    
    //App timmer used to check the RX's end
    TimerInit( &RxAppTimeoutTimer, RxTimeoutTimerIrq );
    TimerSetValue( &RxAppTimeoutTimer, RX_TIMER_TIMEOUT );
    
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );

    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );


    printf("\r\n=============================\r\n");
    printf("SX126X Cad Performance starts\r\n");
    printf("=============================\r\n\r\n");
#if(RX_FW == 1)
    SX126xConfigureCad( CAD_SYMBOL_NUM,
                        CAD_DET_PEAK,CAD_DET_MIN,
                        CAD_TIMEOUT_MS);            // Configure the CAD
                        Radio.StartCad( );          // do the config and lunch first CAD
#if(FULL_DBG)
    printf("CAD\r\n");
#endif
#else
    State = TX;
#endif

    while( i < NB_TRY )
    {
        switch( State )
        {
            case RX_TIMEOUT:
            {
#if(FULL_DBG)
                printf( "RX Timeout\r\n");
#endif
                RxTimeoutCnt++;
                State = START_CAD;
                break;
            }
            case RX_ERROR:
            {
#if(FULL_DBG)
                printf( "RX Error\r\n");
#endif
                RxErrorCnt++;
                PacketReceived = false;
                State = START_CAD;
            break;
            }
            case RX:
            {
                if( PacketReceived == true )
                {
                    PacketReceived = false;     // Reset flag
                    if((Buffer[0]=='C') && (Buffer[1]=='A') && (Buffer[2]=='D'))
                    {
                        PacketCnt = (Buffer[4] << 8) + Buffer[5];   // ID packet
                        RxCorrectCnt++;         // Update RX counter
#if(FULL_DBG)
                        printf( "Rx Packet n %d\r\n", PacketCnt );
#endif
                    }
                State = START_CAD;
                }
                else
                { 
                    if (CadRx == CAD_SUCCESS)
                    {
                        channelActivityDetectedCnt++;   // Update counter
#if(FULL_DBG)
                        printf( "Rxing\r\n");
#endif
                        RxTimeoutTimerIrqFlag = false;
                        TimerReset(&RxAppTimeoutTimer);	// Start the Rx's's Timer
                        Radio.Rx( RX_TIMEOUT_VALUE );   // CAD is detected, Start RX
                    }
                    else
                    {
                        TimerStart(&CADTimeoutTimer);   // Start the CAD's Timer
                    }
                    State = LOWPOWER;
                }           
                break;
            }
            case TX:
            {
                printf("Send Packet n %d \r\n",PacketCnt);

                // Send the next frame
                Buffer[0] = 'C';
                Buffer[1] = 'A';
                Buffer[2] = 'D';
                Buffer[3] = '0';
                Buffer[4] = PacketCnt>>8;
                Buffer[5] = (uint8_t)PacketCnt ;

                if( PacketCnt == 0xFFFF)
                {
                    PacketCnt = 0;
                }
                else
                {
                    PacketCnt ++;
                }
                //Send Frame
                DelayMs( 1 );
                Radio.Send( Buffer, 6 );

                State = LOWPOWER;
                break;
            }
            case TX_TIMEOUT:
            {
                State = LOWPOWER;
                break;
            }
            case START_CAD:
            {
                i++;    // Update NbTryCnt
                TimerStop(&RxAppTimeoutTimer);  // Stop the Rx's Timer
                // Trace for debug
                if(CadRx == CAD_FAIL)
                {
#if(FULL_DBG)
                printf("No CAD detected\r\n");
#endif
                }
                CadRx = CAD_FAIL;           // Reset CAD flag
                DelayMs(randr(10,500));     //Add a random delay for the PER test
#if(FULL_DBG)
                printf("CAD %d\r\n",i);
#endif
                Radio.StartCad( );          //StartCad Again
                State = LOWPOWER;
            break;
            }
            case LOWPOWER:
            default:
                // Set low power
                break;
        }

        TimerLowPowerHandler( );
        // Process Radio IRQ
        Radio.IrqProcess( );
    }
            
    //Result
    Per =  100 - (((float)RxCorrectCnt / i) * 100);
    printf("CAD Performance Result :\r\n");
    printf("CAD SYMBOL = %d | Det_Peak = %d | Det_Min = %d \r\n",(int)pow(2,CAD_SYMBOL_NUM),CAD_DET_PEAK,CAD_DET_MIN); 
    printf("Nb try :%d \r\n",i);
    printf("CAD detected : %d \r\nRX Correct : %d \r\n",channelActivityDetectedCnt,RxCorrectCnt);
    printf("RSSI moy : %d dBm\r\nSNR moy : %d \r\n",RssiMoy,SnrMoy);
    printf("PER : %.1f \r\n",Per);
    printf("Nb RxError :%d \r\nNb RxTimout : %d \r\nNbSymbolTimeout : %d \r\n",RxErrorCnt,RxTimeoutCnt,SymbTimeoutCnt);
    printf("TEST END   \r\n \r\n");

    //Reset All results for next test
    Per = 0.0;
    RxCorrectCnt = 0;
    channelActivityDetectedCnt = 0;
    RxErrorCnt = 0;
    RxTimeoutCnt = 0; 
    SymbTimeoutCnt = 0;
    i=0;
}

void OnTxDone( void )
{
    Radio.Standby( );
    State = TX;
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    Radio.Standby( );
    BufferSize = size;
    memcpy( Buffer, payload, BufferSize );
    RssiValue = rssi;
    SnrValue = snr;
    PacketReceived = true;
    RssiMoy = (((RssiMoy * RxCorrectCnt) + RssiValue) / (RxCorrectCnt + 1));
    SnrMoy = (((SnrMoy * RxCorrectCnt) + SnrValue) / (RxCorrectCnt + 1));
    State = RX;
}

void OnTxTimeout( void )
{
    Radio.Standby( );
    State = TX_TIMEOUT;
}

void OnRxTimeout( void )
{
    Radio.Standby( );
    if( RxTimeoutTimerIrqFlag )
    {
        State = RX_TIMEOUT;
    }
    else
    {
#if(FULL_DBG)
        printf(".");
#endif
        Radio.Rx( RX_TIMEOUT_VALUE );   //  Restart Rx
        SymbTimeoutCnt++;               //  if we pass here because of Symbol Timeout 
        State = LOWPOWER;
    }
}

void OnRxError( void )
{
    Radio.Standby( );
    State = RX_ERROR;
}

void OnCadDone( bool channelActivityDetected)
{
    Radio.Standby( );

    if( channelActivityDetected == true )
    {
        CadRx = CAD_SUCCESS;
    }
    else
    {
        CadRx = CAD_FAIL;
    }
    State = RX;
}

void SX126xConfigureCad( RadioLoRaCadSymbols_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin , uint32_t cadTimeout)
{
    SX126xSetDioIrqParams( 	IRQ_CAD_DONE | IRQ_CAD_ACTIVITY_DETECTED, IRQ_CAD_DONE | IRQ_CAD_ACTIVITY_DETECTED,
                            IRQ_RADIO_NONE, IRQ_RADIO_NONE );
    SX126xSetCadParams( cadSymbolNum, cadDetPeak, cadDetMin, LORA_CAD_ONLY, ((cadTimeout * 1000) / 15.625 ));
}

static void CADTimeoutTimeoutIrq( void )
{
    Radio.Standby( );
    State = START_CAD;
}

static void RxTimeoutTimerIrq( void )
{
    RxTimeoutTimerIrqFlag = true;
}
