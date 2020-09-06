import time
import paho.mqtt.client as paho
import requests
import urllib3

# Callback for MQTT messages
# Forward the received ID to the cloud
def on_message(client, userdata, message):
    time.sleep(0.1)
    print("received message =",str(message.payload.decode("utf-8")))
    urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

    Headers = {'Content-type': 'application/json'}
    url = 'https://raul:MasterIOT!@13.69.123.54/piwebapi/streams/F1DPTBbWLAXLjE6mSMM0g5Tk8glQAAAAQ1ZJTExBTlVBLVBJXDMwMjA4/value'

    request_body = {
      'Value': message.payload.decode("utf-8")
    }
    resp = requests.post(url, json=request_body, headers=Headers, verify=False)

    print(resp.status_code)

client= paho.Client("garbage_truck_backend")
client.on_message=on_message
print("connecting to broker ")
client.connect("192.168.2.118",1883)#connect
client.loop_start() #start loop to process received messages
print("subscribing ")
client.subscribe("garbage_truck/container_id")#subscribe

print("Ready!")

 #Endless loop
do_loop = True
while do_loop:

    try:
        time.sleep(1)
    except KeyboardInterrupt:
        print("Ctrl-c received! Sending kill to threads...")
        client.disconnect()
        client.loop_stop()
        do_loop = False
