# -*- coding: cp1251 -*-
__author__ = 'Tim'


import sys
import xml.etree.ElementTree as ET
import os.path
import os
import shutil

def createMapFile(old_data, new_data, file_name, map_file):
    with open(file_name) as file:
        text = file.read()
        for index, replaced_data in enumerate(old_data):
            text = text.replace('@UPP' + replaced_data, new_data[index].upper())
            text = text.replace('@NORM' + replaced_data, new_data[index].title())
            text = text.replace('@DOWN' + replaced_data, new_data[index].lower())
            text = text.replace(replaced_data, new_data[index])
        with open(map_file, 'w') as file:
            file.write(text)

tree = ET.parse('CodeGeneratorIni.xml')
root = tree.getroot()
libs=root.find('Libraries')
lib_path=libs.find('Path')
print ('Library path: '+lib_path.text)

num_args=len(sys.argv)

print('Component creting script started')

if num_args <3:
    exit(1)

lib_name=sys.argv[1]

namespace_name=sys.argv[2]

component_name=sys.argv[3]

include_file_name='../../../Rdk/Deploy/Include/rdk.h'

if num_args == 5:
   inheritance_name=sys.argv[4]
else:
   inheritance_name='RDK::UNet'


print ('Creating component: '+component_name)

new_path=lib_path.text+lib_name+'/Core/'
print (new_path)

if not os.path.exists(new_path) or os.path.isfile(new_path):
    print ('Error access directory!')
    exit(2)

if os.path.exists(new_path+component_name+'.cpp') or os.path.exists(new_path+component_name+'.h'):
    print ('Files already exists!')
    exit(3)


# Модифицируем шаблоны библиотеки
template_data = ['@NAMESPACE_NAME@', '@HEADER_FILE_NAME@','@CPP_FILE_NAME@', '@CLASS_NAME@', '@LIBRARY_NAME@', '@INCLUDE_FILE_NAME@', '@INHERITANCE_NAME@']
dest_data = [namespace_name, component_name+'.h', component_name+'.cpp', component_name, lib_name, include_file_name, inheritance_name]
createMapFile(template_data,dest_data,'CodeTemplates/UComponentTemplate.h',new_path+component_name+'.h')
createMapFile(template_data,dest_data,'CodeTemplates/UComponentTemplate.cpp',new_path+component_name+'.cpp')

# открытие файла с компонентой в h-нике библиотеки
newText = '#include<' +component_name+'.h'+'> \n'

if os.path.exists(new_path+'U'+lib_name+'.h'):
   with open(new_path+'U'+lib_name+'.h', 'r') as file:
        data = file.readlines()
   data.insert(5,newText)
   with open(new_path+'U'+lib_name+'.h', 'w') as file:
        file.writelines(data)
elif os.path.exists(new_path+'Lib.h'):
   with open(new_path+'Lib.h', 'r') as file:
        data = file.readlines()
   data.insert(5,newText)
   with open(new_path+'Lib.h', 'w') as file:
        file.writelines(data)
else:
    print ('Lib .h file not found!')
    exit(4)

# изменение cpp-шника библиотеки
with open('CodeTemplates/ForCPP.txt') as file:
    text = file.read()
text = text + '\n';
lookup ='//Uppload class here\n'

if os.path.exists(new_path+'U'+lib_name+'.cpp'):
   with open(new_path+'U'+lib_name+'.cpp', 'r') as file:
        data = file.readlines()

   with open(new_path + 'U' + lib_name + '.cpp') as file:
        for num, line in enumerate(file, 1):
            if lookup in line:
               writeHere = num
               data[writeHere] = text
               break

   with open(new_path+'U'+lib_name+'.cpp', 'w') as file:
        file.writelines(data)
elif os.path.exists(new_path+'Lib.cpp'):
   with open(new_path+'Lib.cpp', 'r') as file:
        data = file.readlines()

   with open(new_path + 'Lib.cpp') as file:
        for num, line in enumerate(file, 1):
            if lookup in line:
               writeHere = num
               data[writeHere] = text
               break

   with open(new_path+'Lib.cpp', 'w') as file:
        file.writelines(data)
else:
    print ('Lib .cpp file not found!')
    exit(5)

#with open(new_path + 'U' + lib_name + '.cpp', 'r') as file:
#    data = file.readlines()
#
#with open(new_path + 'U' + lib_name + '.cpp') as file:
#    for num, line in enumerate(file, 1):
#        if lookup in line:
#            writeHere = num
#            data[writeHere] = text
#            break

#with open(new_path+'U'+lib_name+'.cpp', 'w') as file:
#    file.writelines(data)

template_data = ['@CLASS_NAME@', '@SIMPLE_CLASS_NAME@']
if component_name[0] == 'U':
    SimpleComponent_name = component_name[:0] + component_name[(0 + 1):]
    if SimpleComponent_name[0] == 'B':
        SimpleComponent_name = SimpleComponent_name[:0] + SimpleComponent_name[(0 + 1):]
        if SimpleComponent_name[0] == 'A':
            SimpleComponent_name = SimpleComponent_name[:0] + SimpleComponent_name[(0 + 1):]
elif component_name[0] == 'R':
    SimpleComponent_name = component_name[:0] + component_name[(0 + 1):]
    if SimpleComponent_name[0] == 'T':
        SimpleComponent_name = SimpleComponent_name[:0] + SimpleComponent_name[(0 + 1):]
        if SimpleComponent_name[0] == 'V':
            SimpleComponent_name = SimpleComponent_name[:0] + SimpleComponent_name[(0 + 1):]
            if SimpleComponent_name[0] == 'A':
                SimpleComponent_name = SimpleComponent_name[:0] + SimpleComponent_name[(0 + 1):]
else:
     SimpleComponent_name = component_name[:0] + component_name[(0 + 1):]



new_data = [component_name, SimpleComponent_name]

if os.path.exists(new_path+'U'+lib_name+'.cpp'):
   with open(new_path + 'U' + lib_name + '.cpp') as file:
        text = file.read()
        for index, replaced_data in enumerate(template_data):
            text = text.replace('@UPP' + replaced_data, new_data[index].upper())
            text = text.replace(replaced_data, new_data[index])
   with open(new_path + 'U' + lib_name + '.cpp', 'w') as file:
        file.write(text)

elif os.path.exists(new_path+'Lib.cpp'):
   with open(new_path + 'Lib.cpp') as file:
        text = file.read()
        for index, replaced_data in enumerate(template_data):
            text = text.replace('@UPP' + replaced_data, new_data[index].upper())
            text = text.replace(replaced_data, new_data[index])
   with open(new_path + 'Lib.cpp', 'w') as file:
        file.write(text)
else:
    print ('Lib .cpp file not found!')
    exit(6)


#with open(new_path + 'U' + lib_name + '.cpp') as file:
#    text = file.read()
#    for index, replaced_data in enumerate(template_data):
#        text = text.replace('@UPP' + replaced_data, new_data[index].upper())
#        text = text.replace(replaced_data, new_data[index])
#with open(new_path + 'U' + lib_name + '.cpp', 'w') as file:
#    file.write(text)

# добавление h-ника cpp-шника в Qt -> pro файл
lookupH ='HEADERS +='
lookupCPP ='SOURCES +='
with open(lib_path.text+lib_name+'/Build/Qt/'+lib_name+'.pro') as file:
    data = file.readlines()

textH = '\n    ../../Core/' + component_name+'.h'+' \  \n'
textCPP = '\n    ../../Core/' + component_name+'.cpp'+' \  \n'
with open(lib_path.text+lib_name+'/Build/Qt/'+lib_name+'.pro') as file:
    for num, line in enumerate(file, 1):
        if lookupH in line:
            writeHere = num
            data[writeHere] = textH
        if lookupCPP in line:
            writeHere = num
            data[writeHere] = textCPP
            break

with open(lib_path.text+lib_name+'/Build/Qt/'+lib_name+'.pro', 'w') as file:
    file.writelines(data)

#изменение Lib.cpp в Deploy/Include
#lookup ='//#include "../../Core/CPP_FILE_NAME" '
#with open(lib_path.text+lib_name+'/Deploy/Include/Lib.cpp') as file:
#    data = file.readlines()

#text = '\n#include "../../Core/' + component_name+'.cpp"'+'   \n'
#with open(lib_path.text+lib_name+'/Deploy/Include/Lib.cpp') as file:
#    for num, line in enumerate(file, 1):
#        if lookup in line:
#            writeHere = num
#            data[writeHere] = text
#            break

#with open(lib_path.text+lib_name+'/Deploy/Include/Lib.cpp', 'w') as file:
#    file.writelines(data)

#обновление заготовок под компилятор CodeBlocks
lookup ='<Add option="-Wall" />'
with open(lib_path.text+lib_name+'/Build/CodeBlocks/'+lib_name+'.cbp') as file:
    data = file.readlines()

text1 = '\n<Unit filename="../../Core/' + component_name+'.h" />'+'   \n'
text2 = '<Unit filename="../../Core/' + component_name+'.cpp" />'+'   \n'
with open(lib_path.text+lib_name+'/Build/CodeBlocks/'+lib_name+'.cbp') as file:
    for num, line in enumerate(file, 1):
        if lookup in line:
            writeHere = num
            data[writeHere+1] = text1+text2
            break

with open(lib_path.text+lib_name+'/Build/CodeBlocks/'+lib_name+'.cbp', 'w') as file:
    file.writelines(data)

#обновление заготовок под компилятор C++ Builder
lookup ='<ItemGroup>'
with open(lib_path.text+lib_name+'/Build/Bcb/'+lib_name+'.cbproj') as file:
    data = file.readlines()

text1 = '\n<CppCompile Include="../../Core/' + component_name+'.cpp" >'+'   \n </CppCompile> \n'
with open(lib_path.text+lib_name+'/Build/Bcb/'+lib_name+'.cbproj') as file:
    for num, line in enumerate(file, 1):
        if lookup in line:
            writeHere = num
            data[writeHere+1] = text1
            break

with open(lib_path.text+lib_name+'/Build/Bcb/'+lib_name+'.cbproj', 'w') as file:
    file.writelines(data)
