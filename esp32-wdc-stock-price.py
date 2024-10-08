import urequests
import ujson
from machine import Pin, I2C
import ssd1306
import network
import time

# ESP32 Pin assignment for I2C (SCL and SDA pins)
i2c = I2C(0, scl=Pin(22), sda=Pin(21))

# OLED screen setup
oled_width = 128
oled_height = 64
oled = ssd1306.SSD1306_I2C(oled_width, oled_height, i2c)

# Connect to WiFi
def connect_wifi(ssid, password):
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, password)

    while not wlan.isconnected():
        print('Connecting to network...')
        time.sleep(1)
    print('Connected to', ssid)
    print('IP:', wlan.ifconfig()[0])

# Function to get stock price
def get_stock_price(symbol, api_key):
    url = f'https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol={symbol}&interval=1min&apikey={api_key}'
    response = urequests.get(url)
    data = response.json()

    # Extract the most recent stock price
    last_refreshed = data['Meta Data']['3. Last Refreshed']
    stock_price = data['Time Series (1min)'][last_refreshed]['1. open']

    return stock_price

# Display stock price on OLED
def display_stock_price(symbol, price):
    oled.fill(0)  # Clear the display
    oled.text(f'{symbol}: ${price}', 0, 10)
    oled.show()

# Main loop
def main():
    # Replace with your WiFi credentials
    ssid = 'Wokwi-GUEST'
    password = ''
    
    # Alpha Vantage API key
    api_key = '7XM9Q3HGVB3DLXTJ'

    # Stock symbol for Western Digital (WDC)
    stock_symbol = 'WDC'

    connect_wifi(ssid, password)

    try:
        while True:
            stock_price = get_stock_price(stock_symbol, api_key)
            display_stock_price(stock_symbol, stock_price)
            time.sleep(60)  # Update every 60 seconds
    except Exception as e:
        print('Error:', e)
        oled.fill(0)
        oled.text('Error!', 10, 10)
        oled.show()

# Run the program
main()
