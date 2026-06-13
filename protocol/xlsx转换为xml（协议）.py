import csv
import pandas as pd
import xml.etree.ElementTree as ET

fileName = 'fire_alarm_protocol'
deviceName = 'fire_alarm'
# xlsx转csv格式
df = pd.read_excel(fileName + '.xlsx', engine='openpyxl')
df.to_csv(fileName + '.csv', index=False)
# 创建根节点
root = ET.Element(fileName, device=deviceName)

# 添加功能码节点
fc01 = ET.SubElement(root, "function_code01", code="0x01", description="Read Coils")
fc02 = ET.SubElement(root, "function_code02", code="0x02", description="Read Discrete Input")
fc03 = ET.SubElement(root, "function_code03", code="0x03", description="Read Holding Registers")
fc04 = ET.SubElement(root, "function_code04", code="0x04", description="Read Input Registers")
fc05 = ET.SubElement(root, "function_code05", code="0x05", description="Write Single Coil")
fc06 = ET.SubElement(root, "function_code06", code="0x06", description="Write Single Registers")
dido = ET.SubElement(root, "dido", code="", description="Parse dido")

# 添加寄存器节点
with open(fileName + '.csv', 'r', newline='', encoding='utf-8') as csvfile:
    reader = csv.reader(csvfile)
    header = next(reader)
    fcIndex = header.index('功能码')
    adIndex = header.index('地址')
    naIndex = header.index('名字')
    daIndex = header.index('数据类型')
    magIndex = header.index('倍率')
    offsetIndex = header.index('偏移')
    unitIndex = header.index('单位')
    rows = list(reader)
    for row in rows:
        if row[fcIndex] == '1':
            if '0x' in row[adIndex] or '0X' in row[adIndex]:
                reg1 = ET.SubElement(fc01, "coil",
                                    address=str(int(row[adIndex], 16)),
                                    name=row[naIndex],
                                    datatype=row[daIndex],
                                    unit=row[unitIndex],
                                    )
            else:
                reg1 = ET.SubElement(fc01, "coil",
                                    address=str(int(row[adIndex])),
                                    name=row[naIndex],
                                    datatype=row[daIndex],
                                    unit=row[unitIndex],
                                    )
        if row[fcIndex] == '2':
            if '0x' in row[adIndex] or '0X' in row[adIndex]:
                reg2 = ET.SubElement(fc02, "di",
                                    address=str(int(row[adIndex], 16)),
                                    name=row[naIndex],
                                    datatype=row[daIndex],
                                    unit=row[unitIndex],
                                    )
            else:
                reg2 = ET.SubElement(fc02, "di",
                                    address=str(int(row[adIndex])),
                                    name=row[naIndex],
                                    datatype=row[daIndex],
                                    unit=row[unitIndex],
                                    )
        if row[fcIndex] == '3':
            if '0x' in row[adIndex] or '0X' in row[adIndex]:
                reg3 = ET.SubElement(fc03, "hRegister",
                                    address=str(int(row[adIndex], 16)),
                                    name=row[naIndex],
                                    datatype=row[daIndex],
                                    unit=row[unitIndex],
                                    mag=row[magIndex],
                                    offset=row[offsetIndex],
                                    )
            else:
                reg3 = ET.SubElement(fc03, "hRegister",
                                    address=str(int(row[adIndex])),
                                    name=row[naIndex],
                                    datatype=row[daIndex],
                                    unit=row[unitIndex],
                                    mag=row[magIndex],
                                    offset=row[offsetIndex],
                                    )
        if row[fcIndex] == '4':
            if '0x' in row[adIndex] or '0X' in row[adIndex]:
                reg4 = ET.SubElement(fc04, "iRegister",
                                    address=str(int(row[adIndex], 16)),
                                    name=row[naIndex],
                                    datatype=row[daIndex],
                                    unit=row[unitIndex],
                                    mag=row[magIndex],
                                    offset=row[offsetIndex],
                                    )
            else:
                reg4 = ET.SubElement(fc04, "iRegister",
                                    address=str(int(row[adIndex])),
                                    name=row[naIndex],
                                    datatype=row[daIndex],
                                    unit=row[unitIndex],
                                    mag=row[magIndex],
                                    offset=row[offsetIndex],
                                    )
        elif row[fcIndex] == 'na':
            _dido = ET.SubElement(dido, "subdido", name=row[naIndex],level=row[daIndex] )
        # print(row[fcIndex],type(row[fcIndex]))

# 生成 XML 文件
tree = ET.ElementTree(root)
ET.indent(tree, space="\t", level=0)
tree.write(fileName + ".xml", encoding="utf-8", xml_declaration=True)

# tree = ET.tostring(root, encoding='utf-8',xml_declaration=True)
# dom =minidom.parseString(tree)
# prettty_tree = dom.toprettyxml(indent = '   ',newl='\n',encoding='utf-8')
# with open('output2.xml', 'wb') as f:
#     f.write(tree)
