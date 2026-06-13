import xml.etree.ElementTree as ET

tree = ET.parse('n9DCDC_protocol_V1.7.xml')
root = tree.getroot()
fc03 = root.find('function_code03')
dict1 = {}
for hr in fc03.findall('hRegister'):
    dict1[hr.get('name')]={}
    dict1[hr.get('name')]['address'] = int(hr.get('address'))
    dict1[hr.get('name')]['value'] = 0
    dict1[hr.get('name')]['mag'] = int(hr.get('mag'))
    dict1[hr.get('name')]['offset'] = int(hr.get('offset'))
    dict1[hr.get('name')]['datatype'] = hr.get('type')
    dict1[hr.get('name')]['unit'] = hr.get('unit')

print(dict1)
# print(dict1['下设充放电电流（默认）']['address'])
# print(len(dict1.keys()))
# print(int(dict1['DSP 版本1']['address']))
# print(int(dict1['其他故障字屏蔽字']['address'])-int(dict1['DSP 版本1']['address']))

hr = []
for i in range(101):
    hr.append(i)
print(hr)
n1 = dict1['DSP 版本1']['address']
n2 = dict1['低压侧恒压充电电压设置值']['address']
for i, v in zip(range(len(hr)), dict1.values()):
    if v['address'] < n2:
        v['value'] = hr[i + v['address'] - n1] / v['mag'] - v['offset']
        n1 += 1
    elif v['address'] >= n2:
        v['value'] = hr[i + v['address'] - n2] / v['mag'] - v['offset']
        n2 += 1
print(dict1)

# class Device:
#     def __init__(self):
#         self.device_id = 0
#         self.device_type = 0
#         self.device_com = 0

# class Pcs(Device):
#     def __init__(self):
#         super().__init__()
#         self.fc1 = {}
#
#     def speak(self):
#         print('pcs speak')
#
# def main(device:Device):
#     device.speak()
# device01 =Device()
# pcs01 = Pcs()
# print(type(device01))
# print(type(pcs01))
# print(isinstance(pcs01,Device))
#
# main(pcs01)