--ジュラック・ギガノト
function c80032567.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,nil,aux.NonTuner(Card.IsRace,RACE_DINOSAUR),1)
	c:EnableReviveLimit()
	--Atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x22))
	e1:SetValue(c80032567.val)
	c:RegisterEffect(e1)
end
function c80032567.filter(c)
	return c:IsSetCard(0x22) and c:IsType(TYPE_MONSTER)
end
function c80032567.val(e,c)
	return Duel.GetMatchingGroupCount(c80032567.filter,e:GetHandlerPlayer(),LOCATION_GRAVE,0,nil)*200
end
