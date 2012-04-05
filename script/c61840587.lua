--携帯型バッテリー
function c61840587.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c61840587.target)
	e1:SetOperation(c61840587.operation)
	c:RegisterEffect(e1)
	--Destroy
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_SINGLE)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetOperation(c61840587.desop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
	--Destroy2
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_LEAVE_FIELD)
	e3:SetCondition(c61840587.descon2)
	e3:SetOperation(c61840587.desop2)
	e3:SetLabelObject(e1)
	c:RegisterEffect(e3)
end
function c61840587.filter(c,e,tp)
	return c:IsSetCard(0x28) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c61840587.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c61840587.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>1
		and Duel.IsExistingTarget(c61840587.filter,tp,LOCATION_GRAVE,0,2,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c61840587.filter,tp,LOCATION_GRAVE,0,2,2,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,2,0,0)
	if e:GetLabelObject() then e:GetLabelObject():DeleteGroup() end
	e:SetLabelObject(nil)
end
function c61840587.sfilter(c,e,tp)
	return c:IsRelateToEffect(e) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c61840587.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(c61840587.sfilter,nil,e,tp)
	local sct=sg:GetCount()
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if sct==0 or ft<=0 then return end
	if sct>ft then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		sg=sg:Select(tp,ft,ft,nil)
		sct=ft
	end
	Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP_ATTACK)
	local tc=sg:GetFirst()
	c:SetCardTarget(tc)
	tc:CreateRelation(c,RESET_EVENT+0x1020000)
	if sct>1 then
		local tc2=sg:GetNext()
		c:SetCardTarget(tc2)
		tc2:CreateRelation(c,RESET_EVENT+0x1020000)
	end
	e:SetLabelObject(sg)
	sg:KeepAlive()
end
function c61840587.desfilter1(c,rc)
	return c:IsRelateToCard(rc) and c:IsLocation(LOCATION_MZONE) and not c:IsStatus(STATUS_DESTROY_CONFIRMED+STATUS_BATTLE_DESTROYED)
end
function c61840587.desop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local sg=e:GetLabelObject():GetLabelObject()
	if not sg then return end
	Duel.Destroy(sg:Filter(c61840587.desfilter1,nil,c),REASON_EFFECT)
	sg:DeleteGroup()
	e:SetLabelObject(nil)
end
function c61840587.descon2(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local sg=e:GetLabelObject():GetLabelObject()
	if not sg or c:IsStatus(STATUS_DESTROY_CONFIRMED) then return false end
	local rg=eg:Filter(Card.IsRelateToCard,nil,c)
	local tc=rg:GetFirst()
	while tc do sg:RemoveCard(tc) tc=rg:GetNext() end
	return sg:GetCount()==0
end
function c61840587.desop2(e,tp,eg,ep,ev,re,r,rp)
	Duel.Destroy(e:GetHandler(),REASON_EFFECT)
end
