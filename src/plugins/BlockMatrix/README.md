BlockMatrix plugin for Falkon
---
This plugin is heavily inspired by uMatrix.
By default, this plugin blocks nearly all network requests. All requests to
third-party websites and to any JS resource are blocked. The modern web is
mostly broken with these settings. FOR ADVANCED USERS ONLY!

Usage
---
Add a file called "blockmatrix_rules" in the "extenstions" subfolder of your
profile. The syntax is similar to uMatrix:
```
* * * block
* 1st-party * allow
* 1st-party script block
falkon.org 1st-party * allow
* facebook.com * block
```
The first element is the website that is making requests, the second the
website the request would be sent to and the third is the requested resource
type. "*" can't be used for pattern matching.
The resource types are:
- script
- image
- media
- css
- xhr
- frame
- other

TODOs
---
- GUI
- Integration of lists with known tracking websites
- Cookie management
