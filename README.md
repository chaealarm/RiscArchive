# RiscArchive
*English README is not yet.

## 개요
RISC-V 프로세서(CH32V003F4U6)를 이용한 셀프 플래싱 가능한 개발보드 및 예제 코드, 사용방법 등을 소개하는 Repository

## 회로 및 PCB
/PCB의 Gerber, BOM, PickAndPlace를 이용하여 PCB를 주문할 수 있습니다.

/PCB의 JSON 파일들은 EasyEDA Pro에서 열 수 있습니다.

![image](https://github.com/chaealarm/RiscArchive/assets/12396749/0721092c-2056-44c0-808e-987940667b7d)
![image](https://github.com/chaealarm/RiscArchive/assets/12396749/1f0afe7f-4689-4c78-9d91-c1dbc6189adc)

## 예제 코드
/rv003usb/Test_RiscArchive에서 README.md를 참고하세요

RE Aoharu, Endless Carnival 멜로디를 재생하고, 키보드 매크로(CTRL+C, CTRL+V)키를 구현하는 예제(rv003usb, ch32v003fun 사용)

## 펌웨어 업로드를 위한 부트로더
* 기본적으로, 배포한 보드는 셀프 플래싱이 가능한 부트로더가 내장되어 있습니다.
* 별도로 직접 제작하는 등의 경우, /rv003usb/Bootloader_RiscArchive/의 바이너리(.hex)를 부트로더 영역(0x1FFFF000)에 플래싱하세요


- 아래 Arduino, ch32v003fun 코딩 후 펌웨어 업로드(플래싱)을 위해서는 부트로더 진입이 필요합니다.
- 부트로더 진입은 A1핀과 GND핀을 쇼트한 상태로 전원을 넣었을 때 부트로더로 진입됩니다.
- (예제코드의 브레드보드 배치시) 0번 키를 누른 상태로 USB에 연결하면 부트로더로 진입됩니다.

## Arduino로 사용하기(Windows만 가능)
1. Arduino 설치
2. Arduino 실행 후 Preferences → Additional Board Manager URLs 아래 URL 추가 후 OK

https://github.com/chaealarm/arduino-wch32v003/releases/download/Package_Json/package_ch32v003_index.json

4. Board Manager에서 wch 검색후 "WCH Boards by chaealarm" Install
5. Install 완료 되면 아두이노처럼 사용가능(Arduino에서 보드 선택시 CH32V003 선택, PORT는 아무 PORT나 선택하거나, 선택하지 않아도 됨)
* GPIO 핀은 PORTA, PORTC, PORTD에 따라 A1-A2, C0-C7, D0-D7 로 지정
* 펌웨어 업로드 시 상기한 부트로더로 진입하여야 업로드 가능
* USART(Serial)은 해당 핀들이 USB 핀으로 사용되므로 사용 불가
* D1(DIO), D3(USB D+), D4(USB D-), D5(USB Pull-UP) 핀은 GPIO로 사용을 권장하지 않습니다. 

## ch32v003fun을 이용하여 C언어로 코딩하기(Windows만 가능)
* 참고 : https://github.com/cnlohr/ch32v003fun
- https://gnutoolchains.com/risc-v/ 에서 RISC-V GCC10 버전 다운로드 및 설치 https://sysprogs.com/getfile/1107/risc-v-gcc10.1.0.exe
- ch32v003fun을 내려받고, examples의 Makefile을 참고하여 make시 minichlink로 펌웨어 업로드 됨(상기한 부트로더로 진입하여야 업로드 가능)
* 예시) /rv003usb/ch32v003fun/examples/blink.c 를 빌드하여 업로드 하는 경우
  1. /rv003usb/ch32v003fun/examples/ 해당 위치에서 cmd창 실행
  2. RiscArchive 보드를 부트로더 상태로 진입(상기 부트로더 내용 참고)
  3. make 입력 후 엔터
  4. 빌드 및 업로드됨
* USART(Serial)은 해당 핀들이 USB 핀으로 사용되므로 사용 불가
* D1(DIO), D3(USB D+), D4(USB D-), D5(USB Pull-UP) 핀은 GPIO로 사용을 권장하지 않습니다.
