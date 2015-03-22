--ビッグ・シールド・ガードナー
function c65240384.initial_effect(c)
	--to attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_DAMAGE_STEP_END)
	e1:SetOperation(c65240384.posop)
	c:RegisterEffect(e1)
	if not c65240384.global_check then
		c65240384.global_check=true
		local ge=Effect.CreateEffect(c)
		ge:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge:SetCode(EVENT_CHAIN_ACTIVATING)
		ge:SetOperation(c65240384.negop)
		Duel.RegisterEffect(ge,0)
	end
end
function c65240384.posop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c==Duel.GetAttackTarget() and c:IsDefencePos() and c:IsRelateToBattle() then
		Duel.ChangePosition(c,POS_FACEUP_ATTACK)
	end
end
function c65240384.negop(e,tp,eg,ep,ev,re,r,rp)
	if re:IsActiveType(TYPE_SPELL) and re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:IsHasProperty(EFFECT_FLAG_CARD_TARGET) then
		local g=Duel.GetChainInfo(ev,CHAININFO_TARGET_CARDS)
		if not g then return end
		local tc=g:GetFirst()
		if g:GetCount()==1 and tc:IsCode(65240384) and tc:IsLocation(LOCATION_MZONE)
			and tc:IsFacedown() and tc:IsDefencePos() then
			Duel.ChangePosition(tc,POS_FACEUP_DEFENCE)
			Duel.NegateActivation(ev)
		end
	end
end
