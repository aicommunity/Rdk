import glob, os
import xml.etree.ElementTree as ET
from tkinter import *

class CppClass (object):
    def __init__(self, name, parent):
        self.name = name
        self.publicFields = []
        self.signals = []
        self.slots = []
        self.publicMethods = []
        self.parent = parent

clicks = 0
mainList= []
mainList.clear()
secondList= []
secondList.clear()
################################## Нахождение рабочей дирректории ##################################
tree = ET.parse('CodeGeneratorIni.xml')
root = tree.getroot()
libs = root.find('Libraries')
lib_path = libs.find('Path')
print('Library path: ' + lib_path.text)
################################## Конец нахождения рабочей дирректории ##################################



################################## отображает список ##################################
def windowList (list):
    global root
    listbox1.delete(0, 'end')
    for i in list:
        listbox1.insert(END, i)
    root.mainloop()
################################## конец отображения списка ##################################


################################## Нахождение списка всех библиотек ##################################
#return список
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
#return список
def findAllHInLib(path):
    path = path + '/Core'
    cwd = os.getcwd()
    myList = []
    myList.clear()
    if (os.path.exists(path)):
        # нахождение всех h внутри Core
        os.chdir(path)
        for file in glob.glob("*.h"):
            myList.append(file)
        # нахождение в подкаталогах
        fileList = os.listdir(os.getcwd())
        for item in fileList:
            if (os.path.isdir(os.getcwd() + '/' + item)):
                cwd2 = os.getcwd()
                os.chdir(os.getcwd() + '/' + item)
                for file in glob.glob("*.h"):
                    myList.append(file)
                os.chdir(cwd2)
    os.chdir(cwd)
    cwd = os.getcwd()
    return myList
################################## Конец нахождения списка всех заголовочных файлов внутри одной библиотеки##################################


################################## Нахождение списка всех заголовочных файлов ##################################
#return список
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


################################## Нахождение списка классов внутри одного заголовочного файла ##################################
def findClassInH(path):
    cwd = os.getcwd()
    myList = []
    myList.clear()
    lookup = 'class'
    isPublic = 1
    if (os.path.exists(path)):
        # найти конструкцию class *smth* *имя класса*: public *от чего наследуется*
        with open(path) as file:
            for num, line in enumerate(file, 1):
                if lookup in line:
                    # в строке описывается класс
                    listWord = line.split()
                    if listWord[0].find ('//') == -1:
                        #последнее слово в строке - имя родителя
                        parentName = listWord[len(listWord) - 1]
                        if parentName.find('::') != -1:
                            head, sep, tail = parentName.partition('::')
                            parentName = tail
                        for item in listWord:
                            if item.find(':')!= -1:
                                if (len(item) >1 ):
                                    myList.append(CppClass(item.replace(':',''), parentName))
                                else:
                                    newIndx = listWord.index(item)
                                    myList.append(CppClass(listWord[newIndx-1], parentName))

    os.chdir(cwd)
    cwd = os.getcwd()
    return myList
################################## Конец нахождения списка всех классов ##################################


################################## Нахождение списка всех классов из библиотеки ##################################
def findAllClassInLib(path):
    path = path + '/Core'
    cwd = os.getcwd()
    myList = []
    myList.clear()
    if (os.path.exists(path)):
        # нахождение всех h внутри Core
        os.chdir(path)
        for file in glob.glob("*.h"):
            addClassList = findClassInH (os.getcwd()+'/'+file)
            myList= myList + addClassList
        # нахождение в подкаталогах
        fileList = os.listdir(os.getcwd())
        for item in fileList:
            if (os.path.isdir(os.getcwd() + '/' + item)):
                cwd2 = os.getcwd()
                os.chdir(os.getcwd() + '/' + item)
                for file in glob.glob("*.h"):
                    addClassList = findClassInH(os.getcwd() + file)
                    myList = myList + addClassList
                os.chdir(cwd2)
    os.chdir(cwd)
    cwd = os.getcwd()
    return myList
################################## Конец нахождения списка всех классов из библиотеки ##################################


################################## Нахождение списка всех классов ##################################
def findAllClass():
    global lib_path
    libsList = findAllLibs()
    allClassList = []
    allClassList.clear()
    for item in libsList:
        addList=findAllClassInLib(lib_path.text+item)
        allClassList = allClassList + addList
    return allClassList

################################## Конец нахождения списка всех классов##################################

################################## реакции на нажатие кнопок ##################################
def click_button():
    global mainList
    mainList.clear()
    btn1.configure(background="green")
    btn2.configure(background="#555")
    btn3.configure(background="#555")
    btnClean.configure(background="#555")
    global clicks
    mainList =findAllLibs()
    windowList(mainList)
    clicks += 1

def click_button2():
    global lib_path
    global mainList
    btn1.configure(background="#555")
    btn2.configure(background="green")
    btn3.configure(background="#555")
    btnClean.configure(background="#555")
    mainList.clear()
    mainList = findAllH()
    windowList(mainList)
    #print ('list')

def click_button3():
    global mainList
    mainList.clear()
    btn1.configure(background="#555")
    btn2.configure(background="#555")
    btn3.configure(background="green")
    btnClean.configure(background="#555")
    classList = findAllClass()
    for item in classList:
        mainList.append(item.name)
    windowList(mainList)

def clickButtonFind():
    print ('click find')

def clickClean():
    global mainList
    mainList.clear()
    btn1.configure(background="#555")
    btn2.configure(background="#555")
    btn3.configure(background="#555")
    btnClean.configure(background="green")
    root.after(200, lambda: btnClean.configure(background="#555"))
    listbox1.delete(0, 'end')
    listbox2.delete(0, 'end')
################################## конец реакций на нажатие кнопок ##################################

def find(name, path):
    for root, dirs, files in os.walk(path):
        if name in files:
            return os.path.join(root, name)

def onselect1(e):
    global mainList
    indexList = listbox1.curselection()
    if not indexList:
        return
    btn1.configure(background="#555")
    btn2.configure(background="#555")
    btn3.configure(background="#555")
    btnClean.configure(background="#555")
    text = mainList[indexList[0]]
    #если это библиотека, открывать ш-ники
    isLib = text.count('-')
    if isLib > 0:
        mainList.clear()
        mainList = findAllHInLib(lib_path.text+'/'+text)
        windowList(mainList)
    else:
        # если это hник, открывать классы
        isH = text.count('.h')
        if isH > 0:
            mainList.clear()
            path = find(text, lib_path.text)
            classList = findClassInH(path)
            for item in classList:
                mainList.append(item.name)
            windowList(mainList)
        else:
            #если это класс - вывести информацию текстом где-нибудь рядом
            classList = findAllClass()

            for item in classList:
                if (item.name == text):
                    secondList.clear()
                    secondList.append('Имя класса: ')
                    secondList.append(item.name)
                    secondList.append('Имя родителя: ')
                    secondList.append(item.parent)

                    listbox2.delete(0, 'end')
                    for i in secondList:
                        listbox2.insert(END, i)
                    root.mainloop()
                    return
    #listbox1.delete(0, END)
    #listbox1.insert(END, 'Не найдено')
    root.mainloop()

def onselect2(e):
    indexList = listbox2.curselection()
    if not indexList:
        return
    text = secondList[indexList[0]]
    #если это класс - вывести информацию текстом где-нибудь рядом
    classList = findAllClass()
    for item in classList:
        if (item.name == text):
            secondList.clear()
            secondList.append('Имя класса: ')
            secondList.append(item.name)
            secondList.append('Имя родителя: ')
            secondList.append(item.parent)

            listbox2.delete(0, 'end')
            for i in secondList:
                listbox2.insert(END, i)
            root.mainloop()
            return
    listbox2.delete(0, END)
    listbox2.insert(END, 'Не найдено')
    root.mainloop()


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
listbox1.grid(row=1,column=1, rowspan=20)
listbox1.bind('<<ListboxSelect>>', onselect1)

listbox2 = Listbox(root,height=10, width=45, selectmode=EXTENDED)
listbox2.grid(row=3,column=3, rowspan=10)
listbox2.bind('<<ListboxSelect>>', onselect2)

btn1.grid(row=0,column=0)
btn2.grid(row=1,column=0)
btn3.grid(row=2,column=0)
btnClean.grid(row=3,column=0)
#btnFind.grid(row=0,column=3)

root.mainloop()
################################## Конец создания основного окна ##################################
