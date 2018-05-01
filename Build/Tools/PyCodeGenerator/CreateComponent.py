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
            text = text.replace(replaced_data, new_data[index])
        with open(map_file, 'w') as file:
            file.write(text)

tree = ET.parse('CodeGeneratorIni.xml')
root = tree.getroot()
libs=root.find('Libraries')
lib_path=libs.find('Path')
print ('Library path: '+lib_path.text)

num_args=len(sys.argv)

print ('Library creting script started')

if num_args <3:
    exit(0)

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
    exit(0)

if os.path.exists(new_path+component_name+'.cpp') or os.path.exists(new_path+component_name+'.h'):
    print ('Files already exists!')
    exit(0)


# Модифицируем шаблоны библиотеки
template_data = ['@NAMESPACE_NAME@', '@HEADER_FILE_NAME@','@CPP_FILE_NAME@', '@CLASS_NAME@', '@LIBRARY_NAME@', '@INCLUDE_FILE_NAME@', '@INHERITANCE_NAME@']
dest_data = [namespace_name, component_name+'.h', component_name+'.cpp', component_name, lib_name, include_file_name, inheritance_name]
createMapFile(template_data,dest_data,'CodeTemplates/UComponentTemplate.h',new_path+component_name+'.h')
createMapFile(template_data,dest_data,'CodeTemplates/UComponentTemplate.cpp',new_path+component_name+'.cpp')

