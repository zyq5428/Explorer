import time
from io import BytesIO
from PIL import Image
from selenium import webdriver
from selenium.webdriver import ActionChains
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from chaojiying import Chaojiying
import os
from os.path import exists

USERNAME = 'admin'
PASSWORD = 'admin'
CHAOJIYING_USERNAME = 'zyq5428'
CHAOJIYING_PASSWORD = 'qayhz5428'
CHAOJIYING_SOFT_ID = '918592'
CHAOJIYING_KIND = 9004

if not CHAOJIYING_USERNAME or not CHAOJIYING_PASSWORD:
    print("请设置用户名和密码")
    exit()

chromedriver_path = r"D:\python\chromedriver.exe"  #改成你的chromedriver的完整路径地址


class CrackCaptcha():
    def __init__(self):
        self.url = 'https://captcha3.scrape.center/'

        self.username = USERNAME
        self.password = PASSWORD
        self.chaojiying = Chaojiying(CHAOJIYING_USERNAME, CHAOJIYING_PASSWORD,
                                     CHAOJIYING_SOFT_ID)

        self.options = webdriver.ChromeOptions()
        # self.options.add_experimental_option("prefs", {"profile.managed_default_content_settings.images": 2}) # 不加载图片,加快访问速度
        self.options.add_experimental_option(
            'excludeSwitches',
            ['enable-automation'])  # 此步骤很重要，设置为开发者模式，防止被各大网站识别出来使用了Selenium

        self.options.add_experimental_option('useAutomationExtension', False)

        self.options.add_argument('lang=zh-CN,zh,zh-TW,en-US,en')
        self.options.add_argument(
            'user-agent=Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36'
        )
        # self.options.add_argument('user-agent="Mozilla/5.0 (iPod; U; CPU iPhone OS 2_1 like Mac OS X) AppleWebKit/525.18.1 (KHTML, like Gecko) Version/3.1.1 Mobile/5F137 Safari/525.20"')

        self.options.add_argument('--ignore-certificate-errors')
        self.options.add_argument('--ignore-ssl-errors')

        # prefs = {"profile.managed_default_content_settings.images": 2, "webrtc.ip_handling_policy": "disable_non_proxied_udp", "webrtc.multiple_routes_enabled": False, "webrtc.nonproxied_udp_enabled": False}
        prefs = {
            "webrtc.ip_handling_policy": "disable_non_proxied_udp",
            "webrtc.multiple_routes_enabled": False,
            "webrtc.nonproxied_udp_enabled": False
        }

        self.options.add_experimental_option("prefs", prefs)

        # self.options.add_argument('--disable-extensions')
        # self.options.add_experimental_option('excludeSwitches',['ignore-certificate-errors'])
#         self.options.add_argument('--start-maximized')

        self.options.add_argument('disable-blink-features=AutomationControlled'
                                  )  #告诉chrome去掉了webdriver痕迹

        self.browser = webdriver.Chrome(executable_path=chromedriver_path,
                                        options=self.options)
        self.wait = WebDriverWait(self.browser, 20)  #超时时长为10s

    def open(self):
        """
       打开网页输入用户名密码
       :return: None
       """
        self.browser.get(self.url)
        self.browser.implicitly_wait(30) #智能等待，直到网页加载完毕，最长等待时间为30s
        # 填入用户名密码
        username = self.wait.until(
            EC.presence_of_element_located(
                (By.CSS_SELECTOR, 'input[type="text"]')))
        password = self.wait.until(
            EC.presence_of_element_located(
                (By.CSS_SELECTOR, 'input[type="password"]')))
        username.send_keys(self.username)
        time.sleep(1)
        password.send_keys(self.password)
        time.sleep(1)

    def get_captcha_button(self):
        """
       获取初始验证按钮并点击
       :return:
       """
        self.button = self.wait.until(
            EC.presence_of_element_located(
                (By.CSS_SELECTOR, 'button[type="button"]')))
        return self.button

    def get_captcha_element(self):
        """
       获取验证图片对象
       :return: 图片对象
       """
        # 验证码图片加载出来
        try:
            self.wait.until(
                EC.presence_of_element_located(
                    (By.CSS_SELECTOR, 'img.geetest_item_img')))
        except:
            self.button.click()
        # 验证码完整节点
        element = self.wait.until(
            EC.presence_of_element_located(
                (By.CLASS_NAME, 'geetest_panel_box')))
#         print('成功获取验证码节点')
        return element

    def get_captcha_position(self):
        """
       获取验证码位置
       :return: 验证码位置元组
       """
        element = self.get_captcha_element()
        time.sleep(2)
        location = element.location
        size = element.size
        top, bottom, left, right = location['y'], location['y'] + size[
            'height'], location['x'], location['x'] + size['width']
        return (top, bottom, left, right)

    def get_screenshot(self):
        """
       获取网页截图
       :return: 截图对象
       """
        screenshot = self.browser.get_screenshot_as_png()
        screenshot = Image.open(BytesIO(screenshot))
        screenshot.save('screenshot.png')
        return screenshot

    def get_captcha_image(self, name='captcha.png'):
        """
       获取验证码图片
       :return: 图片对象
       """
        top, bottom, left, right = self.get_captcha_position()
        print('验证码位置', top, bottom, left, right)
        screenshot = self.get_screenshot()
        captcha = screenshot.crop((left, top, right, bottom))
        captcha.save(name)
        return captcha

    def get_points(self, captcha_result):
        """
       解析识别结果
       :param captcha_result: 识别结果
       :return: 转化后的结果
       """
        groups = captcha_result.get('pic_str').split('|')
        locations = [[int(number) for number in group.split(',')]
                     for group in groups]
        return locations

    def touch_click_words(self, locations):
        """
       点击验证图片
       :param locations: 点击位置
       :return: None
       """
        for location in locations:
            ActionChains(self.browser).move_to_element_with_offset(
                self.get_captcha_element(), location[0],
                location[1]).click().perform()
            time.sleep(1)
        time.sleep(1)
        self.browser.find_element_by_class_name('geetest_commit_tip').click()

def verification():
    if exists('screenshot.png'):
        os.remove('screenshot.png')
    if exists('captcha.png'):
        os.remove('captcha.png')
        
    image = crack.get_captcha_image()
    
    bytes_array = BytesIO()
    image.save(bytes_array, format='PNG')

    # 识别验证码
    result = crack.chaojiying.PostPic(bytes_array.getvalue(), CHAOJIYING_KIND)
    print(result)

    locations = crack.get_points(result)
    print(locations)

    crack.touch_click_words(locations)
    
    image.close()
    
    # 判定是否成功
    try:
        return crack.wait.until(
            EC.text_to_be_present_in_element((By.TAG_NAME, 'h2'), '登录成功'))
    except:
        print('验证错误')
        crack.chaojiying.ReportError(result.get('pic_id'))
        

if __name__ == '__main__':
    crack = CrackCaptcha()

    crack.open()
    confirm = crack.get_captcha_button()
    time.sleep(10)
    confirm.click()

    second = False
    status = verification()
    time.sleep(3)

    if status:
        print('登录成功')
    else:
        status = verification()
        second = True
    if status:
        print('登录成功')

#     image.close()
    crack.browser.close()