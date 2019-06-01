const express = require('express');
const mongoose = require('mongoose');
const router = express.Router();

//mongoose package schema
const Operation = require('../models/operation');

//Create operation
router.post('/', (req, res, next) => {
    const operation = new Operation({
        OWNER: req.body.OWNER
    });

    operation.save().then(result => {
        res.send(result);
        })
        .catch(err => res.send(err));

    res.status(201).json({
        Operation: operation
    });
});

//Delete operation
router.delete('//:operationID', (req, res, next) =>{
    const id = req.params.operationID;
    Operation.remove({_id: id})
    .exec()
    .then(doc => {
        res.send(doc);
    })
    .catch(err => {
        res.send(err);
    });
});

//Get all operations
router.get('/', (req, res, next) => {
    Operation.find()
    .exec()
    .then(docs => {
        res.send(docs);
    })
    .catch(err => {
        res.send(err);
    });
});

//Get operation by ID
router.get('/:operationID', (req, res, next) => {
    const id = req.params.operationID;
    Operation.find({_id: id}).exec().then(doc => {
        res.send(doc);
    })
    .catch(err => {
        res.send(err);
    })
});

//Push package into operation array
router.put('/:operationId', (req, res, next) => {
    const id = req.params.operationId;
    const pId = req.body.packageId;
    Operation.updateOne({ _id: id }, { $push: {OPERATIONS :[pId]} })
        .exec()
        .then(result => {
            res.send(result);
        })
        .catch(err => {
            res.send(err);
        });
});

//Pop package from operation array
router.patch('/:operationId', (req, res, next) => {
    const id = req.params.operationId;
    Operation.updateOne({ _id: id }, { $pop: {OPERATIONS : 1} })
        .exec()
        .then(result => {
            res.send(result);
        })
        .catch(err => {
            res.send(err);
        });
});

module.exports = router;