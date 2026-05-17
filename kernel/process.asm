[bits 32]

global context_switch ;extern void context_switch(PCB* old_pcb, PCB* new_pcb)
  ;eax
  ;esp+4 - return address
  ;esp+8 first parameter
  ;esp+12 second parameter
  ;4 bytes apart, because a pointer is 4 bytes
context_switch:
  push eax
  mov eax, [esp+8] ; esp+8 now, because we pushed eax
  mov [eax+4], ebx
  mov ebx, [esp]
  mov [eax+0], ebx
  mov [eax+8], ecx
  mov [eax+12],edx
  mov [eax+16],esi
  mov [eax+20],edi
  lea ebx, [esp+4]
  mov [eax+24],ebx
  mov [eax+28],ebp
  mov ebx, [esp+4]
  mov [eax+32],ebx
  pushfd
  pop ebx
  mov [eax+ 36], ebx
  
  ;Load new process
  mov eax, [esp+12] 
  mov ebx, [eax+36]
  push ebx
  popfd
  mov ebx, [eax+4]
  mov ecx, [eax+8]
  mov edx, [eax+12]
  mov esi, [eax+16]
  mov edi, [eax+20]
  mov ebp, [eax+28]
  mov esp, [eax+24]
  mov eax, [eax+0]
  ret
  
