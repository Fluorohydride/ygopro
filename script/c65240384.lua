--ビッグ・シールド・ガードナー
function c65240384.initial_effect(c)
	--to attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_DAMAGE_STEP_END)
	e1:SetOperation(c65240384.posop)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_QUICK_F)
	e2:SetCode(EVENT_CHAINING)
	e2:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DAMAGE_CAL+EFFECT_FLAG_SET_AVAILABLE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c65240384.negcon)
	e2:SetOperation(c65240384.negop)
	c:RegisterEffect(e2)
end
function c65240384.posop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c==Duel.GetAttackTarget() and c:IsDefencePos() and c:IsRelateToBattle() then
		Duel.ChangePosition(c,POS_FACEUP_ATTACK)
	end
end
function c65240384.negcon(e,tp,eg,ep,ev,re,r,rp)
	if re:IsActiveType(TYPE_SPELL) and re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:IsHasProperty(EFFECT_FLAG_CARD_TARGET) then
		local tg=Duel.GetChainInfo(ev,CHAININFO_TARGET_CARDS)
		return tg:GetCount()==1 and tg:GetFirst()==e:GetHandler() and e:GetHandler():IsFacedown()
	else 
		return false 
	end
end
function c65240384.negop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.ChangePosition(e:GetHandler(),POS_FACEUP_DEFENCE) then
		Duel.NegateActivation(ev)
	end
end
