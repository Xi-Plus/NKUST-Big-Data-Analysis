import argparse
import urllib.request
import re


parser = argparse.ArgumentParser()
parser.add_argument('url', nargs='?', default=None)
args = parser.parse_args()


def fetch_and_print(url):
    print(url)
    content = urllib.request.urlopen(url).read().decode()
    emails = re.findall(
        r"(\w[\w!#$%&'*+-/=?^_`{|}~.]*@(?:[A-Za-z0-9-]+\.)+[A-Za-z0-9-]+)", content)
    emails = list(set(emails))
    print(len(emails))
    for email in emails:
        print(email)
    print()


if args.url:
    fetch_and_print(args.url)
else:
    urls = [
        'http://www.csie.kuas.edu.tw/teacher.php',
        'http://www.csie.ncku.edu.tw/ncku_csie/depmember/teacher',
    ]
    for url in urls:
        fetch_and_print(url)
