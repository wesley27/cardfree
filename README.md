# CardFree

A key-chain to replace every card in your wallet.

CardFree is a hackathon project developed by Alex Tolstoy, Callan Cramer, and Wesley Weidenhamer for BitCamp 2018.

# The Scoop
CardFree combines a magnetic stripe reading (MSR) project and an electromagnetic data production project (MagSpoof) into one challenging hackathon project. We researched the protocols of USB systems and HID control to create a python-based driver that can utilize almost every function our MSR device has to offer (read, write, clone, erase, etc.). 

We then built a fully functional version of MagSpoof* and tested it on card readers around our campus. We used our python MSR drivers to obtain the card data we needed to program our hardware. We started with a breadboard, and attempted to miniaturize our hardware project through several prototypes. In the end, our second prototype was our best working device, and using it we were able to successfully gain access to buildings on campus without using our student IDs to swipe. 

# Inspiration
After aquiring a magnetic stripe device, some research on magnetic stripe cards and their uses led us to some really interesting possibilities. Even though magnetic stripe cards seem to be on their way out in favor of more secure means of making transactions and maintaining access, we still found them to be incredibly unique and useful devices that have the potential to be used in many interesting projects.

# Intent
This project is intended to eliminate the need for carrying around credit cards or student IDs. We primarily focused on decoding and reproducing our student IDs. We wanted to create a project that could improve students' convenience, and an improved method of using our student IDs could certainly do the trick.

Our key-chain prototype can be kept on the same ring as your appartment keys, and pulled out at the same time you need them when you're swiping into your dorm. It may not change the world, but it adds a huge convenience to students on campus constantly reaching for their wallets and pulling their ID cards out, and swiping. Our device works with credit cards, gift cards, and other financial cards just as well.

![](https://challengepost-s3-challengepost.netdna-ssl.com/photos/production/software_photos/000/626/337/datas/gallery.jpg)

*MagSpoof is a tool originally developed by Samy Kamkar several years ago for producing magnetic card data via an electromagnetic field, eliminating the need for carrying cards around. It does the same thing as Samsung Pay's MST, which is produced by a chip unique to Samsung phones.
