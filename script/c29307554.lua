--破壊神の系譜
function c29307554.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c29307554.condition)
	e1:SetTarget(c29307554.target)
	e1:SetOperation(c29307554.activate)
	c:RegisterEffect(e1)
	if not c29307554.global_check then
		c29307554.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_DESTROY)
		ge1:SetOperation(c29307554.checkop)
		Duel.RegisterEffect(ge1,0)
	end
end
function c29307554.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	local p1=false
	local p2=false
	while tc do
		if tc:IsLocation(LOCATION_MZONE) and tc:IsDefencePos() then
			if tc:GetReasonPlayer()==0 and tc:IsControler(1) then p1=true end
			if tc:GetReasonPlayer()==1 and tc:IsControler(0) then p2=true end
		end
		tc=eg:GetNext()
	end
	if p1 then Duel.RegisterFlagEffect(0,29307554,RESET_PHASE+PHASE_END,0,1) end
	if p2 then Duel.RegisterFlagEffect(1,29307554,RESET_PHASE+PHASE_END,0,1) end
end
function c29307554.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFlagEffect(tp,29307554)~=0 and Duel.GetTurnPlayer()==tp and Duel.GetCurrentPhase()<=PHASE_BATTLE
		and Duel.GetTurnCount()~=1 and not Duel.IsPlayerAffectedByEffect(tp,EFFECT_CANNOT_BP)
end
function c29307554.filter(c)
	return c:IsFaceup() and c:IsLevelAbove(8) and c:GetEffectCount(EFFECT_EXTRA_ATTACK)==0
end
function c29307554.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c29307554.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c29307554.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c29307554.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c29307554.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_EXTRA_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		e1:SetValue(1)
		tc:RegisterEffect(e1)
	end
end
