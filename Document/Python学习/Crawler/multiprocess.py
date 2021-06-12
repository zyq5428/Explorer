from multiprocessing import Pool
import urllib.request
import urllib.error


def scrape(url):
    try:
        urllib.request.urlopen(url)
        print(f'URL {url} Scraped')
    except (urllib.error.HTTPError, urllib.error.URLError):
        print(f'URL {url} not Scraped')


if __name__ == '__main__':
    pool = Pool(processes=3)
    urls = [
        'https://www.baidu.com',
        'http://www.meituan.com/',
        'http://blog.csdn.net/',
        'http://xxxyxxx.net'
    ]
    pool.map(scrape, urls)
    pool.close()