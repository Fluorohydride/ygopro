--闇のデッキ破壊ウイルス
function c54974237.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_TOHAND)
	e1:SetCost(c54974237.cost)
	e1:SetTarget(c54974237.target)
	e1:SetOperation(c54974237.activate)
	c:RegisterEffect(e1)
end
function c54974237.costfilter(c)
	return c:IsAttribute(ATTRIBUTE_DARK) and c:GetAttack()>=2500
end
function c54974237.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c54974237.costfilter,1,nil,ATTRIBUTE_DARK) end
	local g=Duel.SelectReleaseGroup(tp,c54974237.costfilter,1,1,nil,ATTRIBUTE_DARK)
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
	local g=Duel.GetMatchingGroup(c54974237.tgfilter,tp,0,LOCATION_SZONE,nil,ty)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c54974237.activate(e,tp,eg,ep,ev,re,r,rp)
	local ty=e:GetLabel()
	local conf=Duel.GetFieldGroup(tp,0,LOCATION_SZONE+LOCATION_HAND)
	if conf:GetCount()>0 then
		Duel.ConfirmCards(tp,conf)
		local dg=conf:Filter(Card.IsType,nil,ty)
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
