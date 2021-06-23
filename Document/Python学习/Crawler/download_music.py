import requests
import logging
import re
# import pymongo
from pyquery import PyQuery as pq
from urllib.parse import urljoin
from os import makedirs
from os.path import exists
RESULTS_DIR = '阿城 遍地风流'
exists(RESULTS_DIR) or makedirs(RESULTS_DIR)

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s: %(message)s')

# BASE_URL = 'https://static1.scrape.center'
BASE_URL = 'http://www.335fa.com'
TOTAL_PAGE = 40

def scrape_page(url):
    logging.info('scraping %s...', url)
    try:
        response = requests.get(url)
        if response.status_code == 200:
            return response.text
        logging.error('get invalid status code %s while scraping %s', response.status_code, url)
    except requests.RequestException:
        logging.error('error occurred while scraping %s', url, exc_info=True)
        
def scrape_index(page):
    index_url = f'{BASE_URL}/index.php/vod/play/id/5683/sid/1/nid/{page}.html'
    return scrape_page(index_url)

def download_music(music_name, music_url):
    """下载音乐"""
    response = requests.get(music_url)
    content = response.content
    data_path = f'{RESULTS_DIR}/{music_name}.m4a'
    save_file(data_path, content)


def save_file(filename, content):
   """保存音乐"""
   with open(file=filename, mode="wb") as f:
       f.write(content)

def parse_index(html):
    doc = pq(html)
    link = doc('.ec_playli .detailtj')
    audio = doc('.ec_play')
    name  = re.findall('^\S+ \S+ \S+ \S+ \S+ (.*?)$', link.text())[0]
    audio_url = re.findall('"url":"(.*?)"', audio.text(), re.S)[0]
    audio_url = re.sub(r'\\', '', audio_url)
    logging.info('name: %s, audio: %s', name, audio_url)
    data = {'name':name, 'audio': audio_url}
    download_music(name, audio_url)
        
def main():
    for page in range(1, TOTAL_PAGE + 1):
        index_html = scrape_index(page)
        parse_index(index_html)

if __name__ == '__main__':
    main()
