import 'package:flutter/material.dart';
import 'package:qrapp/home_display.dart';
//import 'dart:async';
//import 'package:firebase_database/firebase_database.dart';
//import 'package:shared_preferences/shared_preferences.dart';

void main() => runApp(new MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'QR Scanner',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: HomeDisplay(),
    );
  }
}