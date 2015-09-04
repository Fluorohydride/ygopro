--諸刃の活人剣術
function c21007444.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c21007444.target)
	e1:SetOperation(c21007444.activate)
	c:RegisterEffect(e1)
end
function c21007444.filter(c,e,tp)
	return c:IsSetCard(0x3d) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c21007444.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:GetControler()==tp and chkc:GetLocation()==LOCATION_GRAVE and c21007444.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>1
		and Duel.IsExistingTarget(c21007444.filter,tp,LOCATION_GRAVE,0,2,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c21007444.filter,tp,LOCATION_GRAVE,0,2,2,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,2,0,0)
end
function c21007444.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(Card.IsRelateToEffect,nil,e)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if sg:GetCount()==0 or ft<=0 then return end
	if ft<sg:GetCount() then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		sg=sg:FilterSelect(tp,c21007444.filter,ft,ft,nil,e,tp)
	end
	if sg:GetCount()>0 then
		local tc=sg:GetFirst()
		local fid=e:GetHandler():GetFieldID()
		while tc do
			if Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEUP_ATTACK) then
				tc:RegisterFlagEffect(21007444,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1,fid)
			end
			tc=sg:GetNext()
		end
		Duel.SpecialSummonComplete()
		sg:KeepAlive()
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
		e1:SetCode(EVENT_PHASE+PHASE_END)
		e1:SetReset(RESET_PHASE+PHASE_END)
		e1:SetCountLimit(1)
		e1:SetCondition(c21007444.descon)
		e1:SetOperation(c21007444.desop)
		e1:SetLabel(fid)
		e1:SetLabelObject(sg)
		Duel.RegisterEffect(e1,tp)
	end
end
function c21007444.desfilter(c,fid)
	return c:GetFlagEffectLabel(21007444)==fid
end
function c21007444.descon(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	if not g:IsExists(c21007444.desfilter,1,nil,e:GetLabel()) then
		g:DeleteGroup()
		e:Reset()
		return false
	else return true end
end
function c21007444.desop(e,tp,eg,ep,ev,re,r,rp)
	local sg=e:GetLabelObject()
	local dg=sg:Filter(c21007444.desfilter,nil,e:GetLabel())
	sg:DeleteGroup()
	if dg:GetCount()>0 then
		local tg1=dg:GetFirst()
		local at1=tg1:GetAttack()
		local tg2=dg:GetNext()
		local at2=0
		local dam=0
		if tg2 then at2=tg2:GetAttack() end
		Duel.Destroy(dg,REASON_EFFECT)
		local og=Duel.GetOperatedGroup()
		if og:IsContains(tg1) then dam=dam+at1 end
		if tg2 and og:IsContains(tg2) then dam=dam+at2 end
		if dam~=0 then Duel.Damage(tp,dam,REASON_EFFECT) end
	end
end
