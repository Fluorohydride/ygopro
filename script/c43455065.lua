--魔力の泉
function c43455065.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCountLimit(1,43455065+EFFECT_COUNT_CODE_OATH)
	e1:SetTarget(c43455065.target)
	e1:SetOperation(c43455065.activate)
	c:RegisterEffect(e1)
end
function c43455065.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_SPELL+TYPE_TRAP)
end
function c43455065.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local ct=Duel.GetMatchingGroupCount(c43455065.filter,tp,0,LOCATION_ONFIELD,nil)
	if chk==0 then return ct>0 and Duel.IsPlayerCanDraw(tp,ct) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(ct)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,ct)
end
function c43455065.activate(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local ct=Duel.GetMatchingGroupCount(c43455065.filter,tp,0,LOCATION_ONFIELD,nil)
	if Duel.Draw(p,ct,REASON_EFFECT)~=0 then
		ct=Duel.GetMatchingGroupCount(c43455065.filter,tp,LOCATION_ONFIELD,0,nil)
		if ct>0 then
			Duel.BreakEffect()
			Duel.DiscardHand(tp,nil,ct,ct,REASON_EFFECT+REASON_DISCARD)
		end
	end
	local rct=1
	if Duel.GetTurnPlayer()~=tp then rct=2 end
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e1:SetTargetRange(0,LOCATION_ONFIELD)
	e1:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e1:SetTarget(c43455065.indtg)
	e1:SetValue(1)
	e1:SetReset(RESET_PHASE+PHASE_END+RESET_OPPO_TURN,rct)
	Duel.RegisterEffect(e1,tp)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e2:SetTargetRange(0,LOCATION_MZONE)
	Duel.RegisterEffect(e2,tp)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_INACTIVATE)
	e2:SetValue(c43455065.efilter)
	e2:SetReset(RESET_PHASE+PHASE_END+RESET_OPPO_TURN,rct)
	Duel.RegisterEffect(e2,tp)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_CANNOT_DISEFFECT)
	e3:SetValue(c43455065.efilter)
	e3:SetReset(RESET_PHASE+PHASE_END+RESET_OPPO_TURN,rct)
	Duel.RegisterEffect(e3,tp)
end
function c43455065.indtg(e,c)
	return c:IsLocation(LOCATION_SZONE) or (c:IsFaceup() and c:IsType(TYPE_SPELL+TYPE_TRAP))
end
function c43455065.efilter(e,ct)
	local te,tp=Duel.GetChainInfo(ct,CHAININFO_TRIGGERING_EFFECT,CHAININFO_TRIGGERING_PLAYER)
	local tc=te:GetHandler()
	return tp~=e:GetHandlerPlayer() and tc:IsType(TYPE_SPELL+TYPE_TRAP)
end
