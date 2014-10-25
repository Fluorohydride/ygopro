--トークン生誕祭
function c97173708.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetLabel(0)
	e1:SetCost(c97173708.cost)
	e1:SetTarget(c97173708.target)
	e1:SetOperation(c97173708.activate)
	c:RegisterEffect(e1)
end
function c97173708.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(100)
	return true
end
function c97173708.rfilter1(c,e,tp)
	local lv=c:GetLevel()
	return lv>0 and c:IsType(TYPE_TOKEN) and Duel.CheckReleaseGroup(tp,c97173708.rfilter2,1,c,lv)
		and Duel.IsExistingTarget(c97173708.spfilter,tp,LOCATION_GRAVE,0,1,nil,e,tp,lv)
end
function c97173708.rfilter2(c,clv)
	local lv=c:GetLevel()
	return lv==clv and c:IsType(TYPE_TOKEN)
end
function c97173708.spfilter(c,e,tp,clv)
	return c:GetLevel()==clv and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c97173708.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c97173708.spfilter(chkc,e,tp,e:GetLabel()) end
	if chk==0 then
		if e:GetLabel()~=100 then return false end
		e:SetLabel(0)
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>-2
			and Duel.CheckReleaseGroup(tp,c97173708.rfilter1,1,nil,e,tp)
	end
	local rg1=Duel.SelectReleaseGroup(tp,c97173708.rfilter1,1,1,nil,e,tp)
	local lv=rg1:GetFirst():GetLevel()
	e:SetLabel(lv)
	local rg2=Duel.SelectReleaseGroup(tp,c97173708.rfilter2,1,9,rg1:GetFirst(),lv)
	rg1:Merge(rg2)
	Duel.Release(rg1,REASON_COST)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft>rg1:GetCount() then ft=rg1:GetCount() end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c97173708.spfilter,tp,LOCATION_GRAVE,0,1,ft,nil,e,tp,lv)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,g:GetCount(),0,0)
end
function c97173708.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	if g:GetCount()==0 then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<g:GetCount() then return end
	local c=e:GetHandler()
	local fid=c:GetFieldID()
	local tc=g:GetFirst()
	while tc do
		Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEUP)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_DISABLE_EFFECT)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e2)
		tc:RegisterFlagEffect(97173708,RESET_EVENT+0x1fe0000,0,1,fid)
		tc=g:GetNext()
	end
	Duel.SpecialSummonComplete()
	g:KeepAlive()
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetCountLimit(1)
	e1:SetLabel(fid)
	e1:SetLabelObject(g)
	e1:SetCondition(c97173708.descon)
	e1:SetOperation(c97173708.desop)
	Duel.RegisterEffect(e1,tp)
end
function c97173708.desfilter(c,fid)
	return c:GetFlagEffectLabel(97173708)==fid
end
function c97173708.descon(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	if not g:IsExists(c97173708.desfilter,1,nil,e:GetLabel()) then
		g:DeleteGroup()
		e:Reset()
		return false
	else return true end
end
function c97173708.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	local tg=g:Filter(c97173708.desfilter,nil,e:GetLabel())
	Duel.Destroy(tg,REASON_EFFECT)
end
