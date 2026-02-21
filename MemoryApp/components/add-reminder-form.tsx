import React, { useState } from 'react';
import { View, TextInput, Button } from 'react-native';
import { Reminder } from '@/types';

interface AddReminderFormProps {
  selectedDate: string;
  onAddReminder: (date: string, reminder: Reminder) => void;
}

export default function AddReminderForm({ selectedDate, onAddReminder }: AddReminderFormProps) {
  const [text, setText] = useState('');
  const [time, setTime] = useState('');

  const handleAdd = () => {
    if (!selectedDate || !text) return;
    onAddReminder(selectedDate, { text, time: time || '00:00' });
    setText('');
    setTime('');
  };

  return (
    <View style={{ marginVertical: 10 }}>
      <TextInput placeholder="Event description" value={text} onChangeText={setText} style={{ borderWidth: 1, padding: 8, marginBottom: 8, borderRadius: 4 }} />
      <TextInput placeholder="Time (HH:MM)" value={time} onChangeText={setTime} style={{ borderWidth: 1, padding: 8, marginBottom: 8, borderRadius: 4 }} />
      <Button title="Add Reminder" onPress={handleAdd} />
    </View>
  );
}