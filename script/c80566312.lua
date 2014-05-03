--祝祷の聖歌
function c80566312.initial_effect(c)
	aux.AddRitualProcGreater(c,aux.FilterBoolFunction(Card.IsCode,56350972))
	--destroy replace
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EFFECT_DESTROY_REPLACE)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetTarget(c80566312.reptg)
	e1:SetValue(c80566312.repval)
	e1:SetOperation(c80566312.repop)
	c:RegisterEffect(e1)
end
function c80566312.repfilter(c,tp)
	return c:IsFaceup() and c:IsControler(tp) and c:IsLocation(LOCATION_MZONE) and c:IsType(TYPE_RITUAL)
		and c:IsReason(REASON_EFFECT+REASON_BATTLE)
end
function c80566312.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemove() and eg:IsExists(c80566312.repfilter,1,nil,tp) end
	return Duel.SelectYesNo(tp,aux.Stringid(80566312,0))
end
function c80566312.repval(e,c)
	return c80566312.repfilter(c,e:GetHandlerPlayer())
end
function c80566312.repop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_EFFECT)
end
