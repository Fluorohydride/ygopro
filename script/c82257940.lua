--プレゼント交換
function c82257940.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c82257940.target)
	e1:SetOperation(c82257940.activate)
	c:RegisterEffect(e1)
end
function c82257940.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAbleToRemove,tp,LOCATION_DECK,0,1,nil)
		and Duel.IsExistingMatchingCard(Card.IsAbleToRemove,tp,0,LOCATION_DECK,1,nil) end
end
function c82257940.activate(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetMatchingGroup(Card.IsAbleToRemove,tp,LOCATION_DECK,0,nil)
	local g2=Duel.GetMatchingGroup(Card.IsAbleToRemove,1-tp,LOCATION_DECK,0,nil)
	if g1:GetCount()==0 or g2:GetCount()==0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local rg1=g1:Select(tp,1,1,nil)
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_REMOVE)
	local rg2=g2:Select(1-tp,1,1,nil)
	rg1:Merge(rg2)
	local c=e:GetHandler()
	local fid=c:GetFieldID()
	Duel.Remove(rg1,POS_FACEDOWN,REASON_EFFECT)
	rg1:GetFirst():RegisterFlagEffect(82257940,RESET_EVENT+0x1fe0000,0,0,fid)
	rg1:GetNext():RegisterFlagEffect(82257940,RESET_EVENT+0x1fe0000,0,0,fid)
	rg1:KeepAlive()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetCountLimit(1)
	e1:SetLabel(fid)
	e1:SetLabelObject(rg1)
	e1:SetCondition(c82257940.thcon)
	e1:SetOperation(c82257940.thop)
	Duel.RegisterEffect(e1,tp)
end
function c82257940.thfilter(c,fid)
	return c:GetFlagEffectLabel(82257940)==fid
end
function c82257940.thcon(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	if g:Filter(c82257940.thfilter,nil,e:GetLabel()):GetCount()<2 then
		g:DeleteGroup()
		return false
	else return true end
end
function c82257940.thop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	local tc1=g:GetFirst()
	local tc2=g:GetNext()
	g:DeleteGroup()
	Duel.SendtoHand(tc1,1-tc1:GetControler(),REASON_EFFECT)
	Duel.SendtoHand(tc2,1-tc2:GetControler(),REASON_EFFECT)
end
