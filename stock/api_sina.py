#coding:utf-8
import urllib2
import re
import math

url='http://hq.sinajs.cn/list=sz000002'
response=urllib2.urlopen(url);
html=response.read()
html=html.replace('"', '').replace(' ','').replace(';','');
str = re.split(' |=|"|,', html)

class StockRealTimeInfo:
	id = None
	name = None

	today_opening_price = None
	yesterday_closing_price = None
	current_price = None
	today_highest_price = None
	today_lowest_price = None
	traded_shares = None
	traded_money = None
	buy1_count = None
	buy1_price = None
	buy2_count = None
	buy2_price = None
	buy3_count = None
	buy3_price = None
	buy4_count = None
	buy4_price = None
	buy5_count = None
	buy5_price = None
	sell1_count = None
	sell1_price = None
	sell2_count = None
	sell2_price = None
	sell3_count = None
	sell3_price = None
	sell4_count = None
	sell4_price = None
	sell5_count = None
	sell5_price = None
	date = None
	time = None

	def __init__(self, id, name,\
	today_opening_price, yesterday_closing_price,\
	current_price,\
	today_highest_price, today_lowest_price,\
	traded_shares, traded_money,\
	buy1_count, buy1_price,\
	buy2_count, buy2_price,\
	buy3_count, buy3_price,\
	buy4_count, buy4_price,\
	buy5_count, buy5_price,\
	sell1_count, sell1_price,\
	sell2_count, sell2_price,\
	sell3_count, sell3_price,\
	sell4_count, sell4_price,\
	sell5_count, sell5_price,\
	date, time):
		self.id                      = id
		self.name                    = name
		self.today_opening_price     = today_opening_price
		self.yesterday_closing_price = yesterday_closing_price
		self.current_price           = current_price
		self.today_highest_price     = today_highest_price
		self.today_lowest_price      = today_lowest_price
		self.traded_shares           = traded_shares
		self.traded_money            = traded_money
		self.buy1_count              = buy1_count
		self.buy1_price              = buy1_price
		self.buy2_count              = buy2_count
		self.buy2_price              = buy2_price
		self.buy3_count              = buy3_count
		self.buy3_price              = buy3_price
		self.buy4_count              = buy4_count
		self.buy4_price              = buy4_price
		self.buy5_count              = buy5_count
		self.buy5_price              = buy5_price
		self.sell1_count             = sell1_count
		self.sell1_price             = sell1_price
		self.sell2_count             = sell2_count
		self.sell2_price             = sell2_price
		self.sell3_count             = sell3_count
		self.sell3_price             = sell3_price
		self.sell4_count             = sell4_count
		self.sell4_price             = sell4_price
		self.sell5_count             = sell5_count
		self.sell5_price             = sell5_price
		self.date                    = date
		self.time                    = time
		
	def show(self):
		print ("[%s] %s:\n\
		Yesterday Closing Price: %s\n\
		Today     Opening Price: %s\n\
		Today     Highest Price: %s\n\
		Today     Lowest  Price: %s\n\
		Traded Money: %s, Traded Shares: %s,\n\
		Sell5 Price: %s, Sell5 Count: %s\n\
		Sell4 Price: %s, Sell4 Count: %s\n\
		Sell3 Price: %s, Sell3 Count: %s\n\
		Sell2 Price: %s, Sell2 Count: %s\n\
		Sell1 Price: %s, Sell1 Count: %s\n\
		Curr  Price: %s\n\
		Buy1  Price: %s, Buy1  Count: %s\n\
		Buy2  Price: %s, Buy2  Count: %s\n\
		Buy3  Price: %s, Buy3  Count: %s\n\
		Buy4  Price: %s, Buy4  Count: %s\n\
		Buy5  Price: %s, Buy5  Count: %s\n"\
		%(self.id, self.name,\
		self.yesterday_closing_price, self.today_opening_price,\
		self.today_highest_price,self.today_lowest_price,\
		self.traded_money, self.traded_shares,\
		self.sell5_price, self.sell5_count,\
		self.sell4_price, self.sell4_count,\
		self.sell3_price, self.sell3_count,\
		self.sell2_price, self.sell2_count,\
		self.sell1_price, self.sell1_count,\
		self.current_price,\
		self.buy1_price, self.buy1_count,\
		self.buy2_price, self.buy2_count,\
		self.buy3_price, self.buy3_count,\
		self.buy4_price, self.buy4_count,\
		self.buy5_price, self.buy5_count))

stock = StockRealTimeInfo(str[0], str[1],\
str[2], str[3],\
str[4],\
str[5], str[6],\
str[9], str[10],\
str[11], str[12],\
str[13], str[14],\
str[15], str[16],\
str[17], str[18],\
str[19], str[20],\
str[21], str[22],\
str[23], str[24],\
str[25], str[26],\
str[27], str[28],\
str[29], str[30],\
str[31], str[32])

print stock
stock.show()