--リミット·リバース
function c27551.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE+TIMING_ATTACK)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c27551.target)
	e1:SetOperation(c27551.operation)
	c:RegisterEffect(e1)
	--Destroy
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_SINGLE)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetOperation(c27551.desop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
	--Destroy2
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_DESTROY)
	e3:SetCondition(c27551.descon2)
	e3:SetOperation(c27551.desop2)
	e3:SetLabelObject(e1)
	c:RegisterEffect(e3)
	--Destroy3
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCode(EVENT_CHANGE_POS)
	e4:SetCondition(c27551.descon3)
	e4:SetOperation(c27551.desop3)
	e4:SetLabelObject(e1)
	c:RegisterEffect(e4)
end
function c27551.filter(c,e,tp)
	return c:IsAttackBelow(1000) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c27551.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c27551.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c27551.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c27551.filter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c27551.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e) then
		if Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP_ATTACK)==0 then return end
		c:SetCardTarget(tc)
		e:SetLabelObject(tc)
		c:CreateRelation(tc,RESET_EVENT+0x1fe0000)
		tc:CreateRelation(c,RESET_EVENT+0x1fe0000)
	end
end
function c27551.desop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=e:GetLabelObject():GetLabelObject()
	if not tc or tc:IsStatus(STATUS_DESTROY_CONFIRMED+STATUS_BATTLE_DESTROYED) then return end
	if tc:IsRelateToCard(c) then
		Duel.Destroy(tc, REASON_EFFECT)
	end
end
function c27551.descon2(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsStatus(STATUS_DESTROY_CONFIRMED) then return false end
	local tc=e:GetLabelObject():GetLabelObject()
	return tc and eg:IsContains(tc) and tc:IsRelateToCard(c) and c:IsRelateToCard(tc)
end
function c27551.desop2(e,tp,eg,ep,ev,re,r,rp)
	Duel.Destroy(e:GetHandler(), REASON_EFFECT)
end
function c27551.descon3(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsStatus(STATUS_DESTROY_CONFIRMED) then return false end
	local tc=e:GetLabelObject():GetLabelObject()
	return tc and eg:IsContains(tc) and tc:IsRelateToCard(c) and c:IsRelateToCard(tc) and tc:IsDefence()
end
function c27551.desop3(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=e:GetLabelObject():GetLabelObject()
	local g=Group.FromCards(tc,c)
	Duel.Destroy(g, REASON_EFFECT)
end
