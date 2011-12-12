--ジュラシックワールド
function c10080320.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--Atk
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e2:SetTarget(aux.TargetBoolFunction(Card.IsRace,RACE_DINOSAUR))
	e2:SetValue(300)
	c:RegisterEffect(e2)
	--Def
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_UPDATE_DEFENCE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e3:SetTarget(aux.TargetBoolFunction(Card.IsRace,RACE_DINOSAUR))
	e3:SetValue(300)
	c:RegisterEffect(e3)
end
