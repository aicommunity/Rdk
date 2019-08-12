import glob, os
import xml.etree.ElementTree as ET
from tkinter import *


clicks = 0

#myList = []

################################## Нахождение рабочей дирректории ##################################
tree = ET.parse('CodeGeneratorIni.xml')
root = tree.getroot()
libs = root.find('Libraries')
lib_path = libs.find('Path')
print('Library path: ' + lib_path.text)
################################## Конец нахождения рабочей дирректории ##################################


################################## Нахождение списка всех классов ##################################
def findAllClass():
    cwd = os.getcwd()
    global lib_path
    os.chdir(lib_path.text)
    for file in glob.glob("*.h"):
        print(file)
################################## Конец нахождения списка всех классов ##################################


################################## отображает список ##################################
def windowList (list):
    global root
    listbox1.delete(0, 'end')
    for i in list:
        listbox1.insert(END, i)
    root.mainloop()
################################## конец отображения списка ##################################


################################## Нахождение списка всех библиотек ##################################
def findAllLibs():
    cwd = os.getcwd()
    myList=[]
    global lib_path
    fileList = os.listdir(lib_path.text)
    for item in fileList:
        isLib = item.count('-')
        if isLib > 0:
            myList.append(item)
    #windowList(myList)
    return myList
################################## Конец нахождения списка всех библиотек ##################################


################################## Нахождение списка всех заголовочных файлов внтри одной библиотеки ##################################
def findAllHInLib(path):
    path = path + '/Core'
    cwd = os.getcwd()
    myList = []
    myList.clear()
    if (os.path.exists(path)):
        #нахождение всех h внутри Core
        os.chdir(path)
        for file in glob.glob("*.h"):
            myList.append(file)
        #нахождение в подкаталогах
        fileList = os.listdir(os.getcwd())
        for item in fileList:
            if (os.path.isdir(os.getcwd()+'/'+item)):
                cwd2 = os.getcwd()
                os.chdir(os.getcwd()+'/'+item)
                for file in glob.glob("*.h"):
                    myList.append(file)
                os.chdir(cwd2)
    os.chdir(cwd)
    cwd = os.getcwd()
    return myList
################################## Конец нахождения списка всех заголовочных файлов внутри одной библиотеки##################################

################################## Нахождение списка всех заголовочных файлов ##################################
def findAllH():
    global lib_path
    #list =findAllHInLib(lib_path.text+'/Rdk-BasicLib')
    listH = []
    list1 = findAllLibs()
    for i in list1:
        path = lib_path.text+i
        items = findAllHInLib(path)
        listH = listH + items
    return listH

################################## Конец нахождения списка всех заголовочных файлов##################################

def click_button():
    btn1.configure(background="green")
    btn2.configure(background="#555")
    btn3.configure(background="#555")
    btnClean.configure(background="#555")
    global clicks
    list =findAllLibs()
    windowList(list)
    clicks += 1

def click_button2():
    global lib_path
    btn1.configure(background="#555")
    btn2.configure(background="green")
    btn3.configure(background="#555")
    btnClean.configure(background="#555")
    #findAllHInLib(lib_path.text+'/Rdk-BasicLib')
    list = findAllH()
    windowList(list)
    #print ('list')

def click_button3():
    btn1.configure(background="#555")
    btn2.configure(background="#555")
    btn3.configure(background="green")
    btnClean.configure(background="#555")
    print ('click')

def clickButtonFind():
    print ('click find')

def clickClean():
    btn1.configure(background="#555")
    btn2.configure(background="#555")
    btn3.configure(background="#555")
    btnClean.configure(background="green")
    root.after(200, lambda: btnClean.configure(background="#555"))
    listbox1.delete(0, 'end')

################################## Создание основного окна ##################################
root = Tk()
root.title("GUI на Python")
root.geometry("800x800")

root.buttonText = StringVar()
root.buttonText.set("Clicks {}".format(clicks))

btn1 = Button(text="Список всех библиотек", background="#555", foreground="#ccc",
             padx="48", pady="8", font="16", command=click_button)
btn2 = Button(text="Список всех заголовочных файлов", background="#555", foreground="#ccc",
             padx="5", pady="8", font="16", command=click_button2)
btn3 = Button(text="Список всех классов", background="#555", foreground="#ccc",
             padx="56", pady="8", font="16", command=click_button3)
btnFind = Button(text="Найти", background="#555", foreground="#ccc",
             padx="10", pady="2", font="16", command=clickButtonFind)
btnClean = Button(text="Отчистить", background="#555", foreground="#ccc",
             padx="95", pady="2", font="16", command=clickClean)


listbox1 = Listbox(root,height=35, width=45, selectmode=EXTENDED)
listbox1.grid(row=1,column=2, rowspan=20)

btn1.grid(row=0,column=0)
btn2.grid(row=1,column=0)
btn3.grid(row=2,column=0)
btnClean.grid(row=3,column=0)
btnFind.grid(row=0,column=3)

root.mainloop()
################################## Конец создания основного окна ##################################
