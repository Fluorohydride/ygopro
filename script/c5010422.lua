--占術姫ウィジャモリガン
function c5010422.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetOperation(c5010422.flipop)
	c:RegisterEffect(e1)
end
function c5010422.flipop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetCountLimit(1)
	e1:SetOperation(c5010422.desop)
	e1:SetReset(RESET_PHASE+RESET_END)
	Duel.RegisterEffect(e1,tp)
end
function c5010422.desfilter(c)
	return c:IsDefencePos() and c:IsDestructable()
end
function c5010422.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c5010422.desfilter,tp,0,LOCATION_MZONE,nil)
	if g:GetCount()>0 then
		Duel.Hint(HINT_CARD,0,5010422)
		local ct=Duel.Destroy(g,REASON_EFFECT)
		Duel.Damage(1-tp,ct*500,REASON_EFFECT)
	end
end
