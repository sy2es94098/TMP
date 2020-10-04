# -*- coding: utf-8 -*-

import os
import tempfile
import heapq
import sys
import shutil
import math

PATH = './process/'

class heapNode():
    def __init__( self, value, file_input):
        self.__value = value
        self.__file_input = file_input

    def get_value( self ):
        return self.__value

    def get_file_input( self ):
        return self.__file_input

class minHeap():
    def __init__( self, output_file, buffer_size):
        self.__data = []
        self.__output_file = output_file
        self.__buffer_size = buffer_size

    def push( self , heapNode ):
        heapq.heappush(self.__data, (heapNode.get_value(), heapNode.get_file_input()))

    def pop( self ):
        node = heapq.heappop(self.__data)
        return heapNode(node[0],node[1])

    def get_len( self ):
        return len(self.__data)

    def sort( self ):
        sorted_file = open(self.__output_file, 'w+')
        open_files = []
        output = ""
        counter = 0
        for f in os.listdir(PATH):
            if os.path.isfile(PATH+f):
                proc_file = open(PATH+f)
                open_files.append(proc_file)
                val = proc_file.readline()
                node = heapNode(int(val),proc_file)
                self.push(node)
        while(self.get_len() > 0 ):
            min_element = self.pop()
            output = output + str(min_element.get_value()) + '\n'
            counter = counter + 1
            if(counter == self.__buffer_size):
                sorted_file.write(output)
                output = ""
                counter = 0
            next_str = min_element.get_file_input().readline()
            if next_str:
                node = heapNode(int(next_str),min_element.get_file_input())
                self.push(node)
            else:
                min_element.get_file_input().close()
    
        sorted_file.write(output)
        output = ""
        counter = 0
        sorted_file.close()

def flush(file_name,data):
    with open(file_name,'w') as f:
        for e in data:
            f.write(str(e)+ "\n")

def read_data(file_name,buffer_size):
    data = []
    if buffer_size <=0:
        return data
    while(True):
        num =  file_name.readline()
        if(num):
            data.append(int(num))
            if(len(data) >= buffer_size):
                break
        else:       
            break
    return data
            
def divide(file_name, buffer_size):
    data = []
    counter = 1
    if os.path.exists(PATH):
        shutil.rmtree(PATH)
    os.mkdir(PATH)

    with open(file_name) as f:
        data = read_data(f, buffer_size)
        while(len(data) > 0):
            data.sort()
            flush(PATH +"/sorted_" + str(counter),data)
            data = read_data(f, buffer_size)
            counter = counter + 1

def remove_process_data():
    if os.path.exists(PATH):
        shutil.rmtree(PATH)

def external_sort(input_file,output_file,buffer_size = math.pow(2,25)):
    divide(input_file,buffer_size)
    heap = minHeap(output_file,buffer_size)
    heap.sort()
    print('Sorted values are written to' , str(output_file))
    remove_process_data()   

if __name__ == "__main__":
    input_file="input.txt"
    external_sort(input_file,"output.txt")

