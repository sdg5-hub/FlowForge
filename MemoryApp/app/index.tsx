import React, { useState } from 'react';
import { ScrollView } from 'react-native';
import Clock from '@/components/clock';
import CalendarComponent from '@/components/calendar-component';
import AddReminderForm from '@/components/add-reminder-form';
import DateCard from '@/components/date-card';
import { Reminder } from '@/types';

export default function HomeScreen() {
  const [selectedDate, setSelectedDate] = useState('');
  const [reminders, setReminders] = useState<Record<string, Reminder[]>>({
    '2026-02-21': [
      { time: '08:30 AM', text: 'Take medicine' },
      { time: '12:00 PM', text: 'Call caregiver' },
    ],
    '2026-02-22': [
      { time: '10:00 AM', text: 'Walk in park' },
      { time: '03:30 PM', text: 'Drink water' },
    ],
  });

  const handleAddReminder = (date: string, reminder: Reminder) => {
    setReminders((prev) => {
      const updated = { ...prev };
      if (!updated[date]) updated[date] = [];
      updated[date].push(reminder);
      return updated;
    });
  };

  return (
    <ScrollView style={{ flex: 1, padding: 10 }}>
      <Clock />
      <CalendarComponent reminders={reminders} selectedDate={selectedDate} onDateSelect={setSelectedDate} />
      {selectedDate && <AddReminderForm selectedDate={selectedDate} onAddReminder={handleAddReminder} />}
      {selectedDate && <DateCard date={selectedDate} reminders={reminders[selectedDate] || []} />}
    </ScrollView>
  );
}