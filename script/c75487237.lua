--ミドル・シールド・ガードナー
function c75487237.initial_effect(c)
	--turn set
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(75487237,0))
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTarget(c75487237.target)
	e1:SetOperation(c75487237.operation)
	c:RegisterEffect(e1)
	if not c75487237.global_check then
		c75487237.global_check=true
		local ge=Effect.CreateEffect(c)
		ge:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge:SetCode(EVENT_CHAIN_ACTIVATING)
		ge:SetOperation(c75487237.negop)
		Duel.RegisterEffect(ge,0)
	end
end
function c75487237.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsCanTurnSet() and c:GetFlagEffect(75487237)==0 end
	c:RegisterFlagEffect(75487237,RESET_EVENT+0x1fc0000+RESET_PHASE+PHASE_END,0,1)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,c,1,0,0)
end
function c75487237.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		Duel.ChangePosition(c,POS_FACEDOWN_DEFENCE)
	end
end
function c75487237.negop(e,tp,eg,ep,ev,re,r,rp)
	if re:IsActiveType(TYPE_SPELL) and re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:IsHasProperty(EFFECT_FLAG_CARD_TARGET) then
		local g=Duel.GetChainInfo(ev,CHAININFO_TARGET_CARDS)
		if not g then return end
		local tc=g:GetFirst()
		if g:GetCount()==1 and tc:IsCode(75487237) and tc:IsLocation(LOCATION_MZONE)
			and tc:IsFacedown() and tc:IsDefencePos() then
			Duel.ChangePosition(tc,POS_FACEUP_DEFENCE)
			Duel.NegateActivation(ev)
		end
	end
end
