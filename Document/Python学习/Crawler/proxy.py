import requests
import asyncio
import aiohttp

# 获取免费IP池 IP
def get_proxy():
    return requests.get("http://127.0.0.1:5010/get/").json()

def delete_proxy(proxy):
    requests.get("http://127.0.0.1:5010/delete/?proxy={}".format(proxy))

# 获取购买的IP池 IP，一段时间后需要在芝麻代理中换API链接
def get_my_proxy():
    try:
        proxy_json = requests.get("http://webapi.http.zhimacangku.com/getip?num=1&type=2&pro=&city=0&yys=0&port=1&time=1&ts=0&ys=0&cs=0&lb=1&sb=0&pb=4&mr=1&regions=").json()
        proxy_data = proxy_json.get('data')[0]
    except Exception:
        print('需要更换代理API接口')
        return ''
    proxy = proxy_data.get('ip') + ':' + str(proxy_data.get('port'))
    return proxy


async def main():
    async with aiohttp.ClientSession() as session:
        async with session.get('https://httpbin.org/get', proxy=proxy) as response:
            print(await response.text())
            
if __name__ == '__main__':
    # proxy = get_my_proxy()
    proxy = get_proxy().get("proxy")
    # proxy = 'http://118.123.40.82'
    proxy = 'http://' + proxy
    print(proxy)
    asyncio.get_event_loop().run_until_complete(main())