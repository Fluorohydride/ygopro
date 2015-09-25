--勝利の導き手フレイヤ
function c12398280.initial_effect(c)
	--atk def
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetTarget(c12398280.tg)
	e1:SetValue(400)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e2)
	--cannot be battle target
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e1:SetCondition(c12398280.con)
	e1:SetValue(aux.imval1)
	c:RegisterEffect(e1)
end
function c12398280.tg(e,c)
	return c:IsRace(RACE_FAIRY)
end
function c12398280.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_FAIRY) and not c:IsCode(12398280)
end
function c12398280.con(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c12398280.filter,c:GetControler(),LOCATION_MZONE,0,1,c)
end
