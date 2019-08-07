# -*- coding: cp1251 -*-
__author__ = 'Tim'


import sys
import xml.etree.ElementTree as ET
import os.path
import os
import shutil
#import commands

def createMapFile(old_data, new_data, file_name, map_file):
    with open(file_name) as file:
        text = file.read()
        for index, replaced_data in enumerate(old_data):
            text = text.replace('@UPP'+replaced_data, new_data[index].upper())
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

if num_args != 3:
    exit(0)

lib_name=sys.argv[1]

# Проверяем окончание имени библиотеки, и добавляем Lib, если нет окончания Lib или Library
if (lib_name.rfind("Lib") != len(lib_name)-3) and (lib_name.rfind("Library") != len(lib_name)-6):
    lib_name+="Lib"

namespace_name=sys.argv[2]

print ('Creating library: '+lib_name)

new_path=lib_path.text+namespace_name.capitalize()+'-'+lib_name
print (new_path)

if os.path.exists(new_path) or os.path.isfile(new_path):
    print ('Error creating directory!')
    exit(0)

# Создаем основную папку библиотеки
os.makedirs(new_path)

# Создаем репозиторий (требуется hg)
os.system('hg init '+new_path)

# Создаем внутренние папки библиотеки
os.makedirs(new_path+'/Bin')
os.makedirs(new_path+'/Core')
os.makedirs(new_path+'/Deploy/Include')

# Копируем шаблоны
shutil.copy('LibProjects/.hgignore',new_path+'/.hgignore')
shutil.copytree('LibProjects/Build', new_path+'/Build')


# Модифицируем шаблоны библиотеки
template_data = ['@NAMESPACE_NAME@', '@HEADER_FILE_NAME@','@CPP_FILE_NAME@', '@CLASS_NAME@', '@LIBRARY_NAME@']
dest_data = [namespace_name, 'U'+lib_name+'.h', 'U'+lib_name+'.h', 'U'+lib_name, lib_name]


createMapFile(template_data, dest_data, 'CodeTemplates/ULibraryTemplate.cpp', new_path+'/Core/'+'U'+lib_name+'.cpp')
createMapFile(template_data, dest_data, 'CodeTemplates/ULibraryTemplate.cpp', new_path+'/Core/'+'U'+lib_name+'.h')

#shutil.copyfile('CodeTemplates/ULibraryTemplate.h', new_path+'/Core/U'+lib_name='.h')
#shutil.copyfile('CodeTemplates/ULibraryTemplate.cpp', new_path+'/Core/U'+lib_name='.cpp')

# Модифицируем файлы для сборки
os.rename(new_path+'/Build/Bcb/Rdk-BasicLib.cbproj', new_path+'/Build/Bcb/'+namespace_name.capitalize()+'-'+lib_name+'.cbproj')
os.rename(new_path+'/Build/CodeBlocks/Rdk-BasicLib.cbp', new_path+'/Build/CodeBlocks/'+namespace_name.capitalize()+'-'+lib_name+'.cbp')
os.rename(new_path+'/Build/Vs/Rdk-BasicLib.vcxproj', new_path+'/Build/Vs/'+namespace_name.capitalize()+'-'+lib_name+'.vcxproj')
os.rename(new_path+'/Build/Qt/Qt.pro', new_path+'/Build/Qt/'+namespace_name.capitalize()+'-'+lib_name+'.pro')

#создание заготовок под компилятор Qt
createMapFile(template_data, dest_data, 'CodeTemplates/Qt.pro', new_path+'/Build/Qt/'+namespace_name+'-'+lib_name+'.pro')


#создание Lib.h и Lib.cpp в Deploy/Include
createMapFile(template_data, dest_data, 'CodeTemplates/Lib.h', new_path+'/Deploy/Include/'+'Lib.h')
createMapFile(template_data, dest_data, 'CodeTemplates/Lib.cpp', new_path+'/Deploy/Include/'+'Lib.cpp')