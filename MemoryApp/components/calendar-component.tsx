import React, { useState } from 'react';
import { View, ScrollView } from 'react-native';
import { Calendar } from 'react-native-calendars';
import DateCard from '@/components/date-card';
import { Reminder } from '@/types';

// Example reminders (event data)
const remindersData: Record<string, Reminder[]> = {
  '2026-02-21': [
    { time: '08:30 AM', text: 'Take medicine' },
    { time: '12:00 PM', text: 'Call caregiver' },
  ],
  '2026-02-22': [
    { time: '10:00 AM', text: 'Walk in park' },
    { time: '03:30 PM', text: 'Drink water' },
  ],
};

export default function CalendarComponent() {
  const [selectedDate, setSelectedDate] = useState<string>('');

  // Generate markedDates with event highlights and selected date
  const getMarkedDates = () => {
    const marks: Record<
      string,
      {
        customStyles: {
          container: object;
          text: object;
        };
      }
    > = {};

    Object.keys(remindersData).forEach((date) => {
      marks[date] = {
        customStyles: {
          container: {
            backgroundColor: '#cce5ff', // light blue for event
            borderRadius: 0,
            width: '100%',
            height: '100%',
            justifyContent: 'center',
            alignItems: 'center',
          },
          text: {
            color: 'black',
            fontWeight: 'bold',
          },
        },
      };
    });

    // Always overwrite the selected date if any
    if (selectedDate) {
      marks[selectedDate] = {
        customStyles: {
          container: {
            backgroundColor: '#0056b3', // dark blue for selection
            borderRadius: 0,
            width: '100%',
            height: '100%',
            justifyContent: 'center',
            alignItems: 'center',
          },
          text: {
            color: 'white',
            fontWeight: 'bold',
          },
        },
      };
    }

    return marks;
  };

  return (
    <ScrollView style={{ flex: 1 }}>
      <Calendar
        onDayPress={(day) => setSelectedDate(day.dateString)}
        markingType="custom"
        markedDates={getMarkedDates()}
      />
      {selectedDate && (
        <DateCard
          date={selectedDate}
          reminders={remindersData[selectedDate] || []}
        />
      )}
    </ScrollView>
  );
}