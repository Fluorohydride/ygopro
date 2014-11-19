--闇のデッキ破壊ウイルス
function c54974237.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_TOHAND+TIMING_END_PHASE)
	e1:SetCost(c54974237.cost)
	e1:SetTarget(c54974237.target)
	e1:SetOperation(c54974237.activate)
	c:RegisterEffect(e1)
end
function c54974237.costfilter(c)
	return c:IsAttribute(ATTRIBUTE_DARK) and c:GetAttack()>=2500
end
function c54974237.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c54974237.costfilter,1,nil) end
	local g=Duel.SelectReleaseGroup(tp,c54974237.costfilter,1,1,nil)
	Duel.Release(g,REASON_COST)
end
function c54974237.tgfilter(c,ty)
	return c:IsFaceup() and c:IsType(ty) and c:IsDestructable()
end
function c54974237.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(54974237,2))
	local ac=Duel.SelectOption(tp,aux.Stringid(54974237,0),aux.Stringid(54974237,1))
	local ty=TYPE_SPELL
	if ac==1 then ty=TYPE_TRAP end
	e:SetLabel(ty)
	local g=Duel.GetMatchingGroup(c54974237.tgfilter,tp,0,LOCATION_ONFIELD,nil,ty)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c54974237.cffilter(c)
	return c:IsLocation(LOCATION_HAND) or (c:IsFacedown() and c:IsType(TYPE_SPELL+TYPE_TRAP))
end
function c54974237.activate(e,tp,eg,ep,ev,re,r,rp)
	local ty=e:GetLabel()
	local g=Duel.GetFieldGroup(tp,0,LOCATION_ONFIELD+LOCATION_HAND)
	if g:GetCount()>0 then
		local cg=g:Filter(c54974237.cffilter,nil)
		Duel.ConfirmCards(tp,cg)
		local dg=g:Filter(Card.IsType,nil,ty)
		Duel.Destroy(dg,REASON_EFFECT)
		Duel.ShuffleHand(1-tp)
	end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetProperty(EFFECT_FLAG_DELAY)
	e1:SetCode(EVENT_DRAW)
	e1:SetOperation(c54974237.desop)
	e1:SetLabel(ty)
	e1:SetReset(RESET_PHASE+PHASE_END+RESET_OPPO_TURN,3)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(e:GetHandler())
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_PHASE+PHASE_END)
	e2:SetCountLimit(1)
	e2:SetCondition(c54974237.turncon)
	e2:SetOperation(c54974237.turnop)
	e2:SetReset(RESET_PHASE+PHASE_END+RESET_OPPO_TURN,3)
	Duel.RegisterEffect(e2,tp)
	e2:SetLabelObject(e1)
	e:GetHandler():RegisterFlagEffect(1082946,RESET_PHASE+PHASE_END+RESET_OPPO_TURN,0,3)
	c54974237[e:GetHandler()]=e2
end
function c54974237.desop(e,tp,eg,ep,ev,re,r,rp)
	if ep==e:GetOwnerPlayer() then return end
	local hg=eg:Filter(Card.IsLocation,nil,LOCATION_HAND)
	if hg:GetCount()==0 then return end
	Duel.ConfirmCards(1-ep,hg)
	local dg=hg:Filter(Card.IsType,nil,e:GetLabel())
	Duel.Destroy(dg,REASON_EFFECT)
	Duel.ShuffleHand(ep)
end
function c54974237.turncon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp
end
function c54974237.turnop(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetLabel()
	ct=ct+1
	e:SetLabel(ct)
	e:GetHandler():SetTurnCounter(ct)
	if ct==3 then
		e:GetLabelObject():Reset()
		e:GetOwner():ResetFlagEffect(1082946)
	end
end
