--緊急発進
function c83054225.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c83054225.spcon)
	e1:SetTarget(c83054225.sptg)
	e1:SetOperation(c83054225.spop)
	c:RegisterEffect(e1)
end
function c83054225.cfilter(c)
	return not c:IsType(TYPE_TOKEN)
end
function c83054225.spcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)>Duel.GetMatchingGroupCount(c83054225.cfilter,tp,LOCATION_MZONE,0,nil)
end
function c83054225.spfilter(c,e,tp)
	return c:IsSetCard(0x101b) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c83054225.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if Duel.GetFlagEffect(tp,83054225)~=0 or Duel.GetLocationCount(tp,LOCATION_MZONE)<0 then return false end
		local ct=Duel.GetMatchingGroupCount(c83054225.spfilter,tp,LOCATION_DECK,0,nil,e,tp)
		return ct>0 and Duel.CheckReleaseGroup(tp,Card.IsCode,1,nil,31533705)
	end
	local ct=Duel.GetMatchingGroupCount(c83054225.spfilter,tp,LOCATION_DECK,0,nil,e,tp)
	local g=Duel.SelectReleaseGroup(tp,Card.IsCode,1,ct,nil,31533705)
	Duel.Release(g,REASON_COST)
	e:SetLabel(g:GetCount())
	Duel.RegisterFlagEffect(tp,83054225,RESET_PHASE+PHASE_END,EFFECT_FLAG_OATH,1)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,g:GetCount(),tp,LOCATION_DECK)
end
function c83054225.spop(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetLabel()
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<ct then return end
	local g=Duel.GetMatchingGroup(c83054225.spfilter,tp,LOCATION_DECK,0,nil,e,tp)
	if g:GetCount()<ct then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local sg=g:Select(tp,ct,ct,nil)
	local fid=e:GetHandler():GetFieldID()
	local tc=sg:GetFirst()
	while tc do
		Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEUP)
		tc:RegisterFlagEffect(83054225,RESET_EVENT+0x1fe0000,0,1,fid)
		tc=sg:GetNext()
	end
	Duel.SpecialSummonComplete()
	sg:KeepAlive()
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetCountLimit(1)
	e1:SetLabel(fid)
	e1:SetLabelObject(sg)
	e1:SetCondition(c83054225.retcon)
	e1:SetOperation(c83054225.retop)
	Duel.RegisterEffect(e1,tp)
end
function c83054225.retfilter(c,fid)
	return c:GetFlagEffectLabel(83054225)==fid
end
function c83054225.retcon(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	if not g:IsExists(c83054225.retfilter,1,nil,e:GetLabel()) then
		g:DeleteGroup()
		e:Reset()
		return false
	else return true end
end
function c83054225.retop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	local tg=g:Filter(c83054225.retfilter,nil,e:GetLabel())
	Duel.SendtoDeck(tg,nil,2,REASON_EFFECT)
end
