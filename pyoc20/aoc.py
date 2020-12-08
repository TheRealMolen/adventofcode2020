import browser_cookie3
import requests

cookies = browser_cookie3.chrome(domain_name='.adventofcode.com')
response = requests.get('https://adventofcode.com/2020/day/7/input', verify=False, cookies=cookies, timeout=3)
print( response.text )