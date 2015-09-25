--ガリトラップ－ピクシーの輪－
function c46502013.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--cannot be battle target
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_SELECT_BATTLE_TARGET)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(0,LOCATION_MZONE)
	e2:SetCondition(c46502013.con)
	e2:SetValue(c46502013.atlimit)
	c:RegisterEffect(e2)
end
function c46502013.con(e)
	return Duel.IsExistingMatchingCard(Card.IsPosition,e:GetHandlerPlayer(),LOCATION_MZONE,0,2,nil,POS_FACEUP_ATTACK)
end
function c46502013.tfilter(c,atk)
	return c:IsFaceup() and c:GetAttack()<atk
end
function c46502013.atlimit(e,c)
	return c:IsFaceup() and not Duel.IsExistingMatchingCard(c46502013.tfilter,c:GetControler(),LOCATION_MZONE,0,1,c,c:GetAttack())
end
