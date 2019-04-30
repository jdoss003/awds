const mongoose = require('mongoose');

const packageSchema = mongoose.Schema({
    packageID: {
        type: String,
        unique: true,
        required: true,
    },
    createdDate: {
        type: Date,
        default: Date.now,
    },
    details: {
        type: String,
        required: true,
    }
});

module.exports = mongoose.model('Package', packageSchema);