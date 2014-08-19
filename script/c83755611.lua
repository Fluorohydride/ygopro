--輝竜星－ショウフク
function c83755611.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,nil,aux.NonTuner(Card.IsRace,RACE_WYRM),1)
	c:EnableReviveLimit()
	--mat check
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_MATERIAL_CHECK)
	e1:SetValue(c83755611.matcheck)
	c:RegisterEffect(e1)
	--todeck
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(83755611,0))
	e2:SetCategory(CATEGORY_TODECK)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCondition(c83755611.tdcon)
	e2:SetTarget(c83755611.tdtg)
	e2:SetOperation(c83755611.tdop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
	--destroy
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(83755611,1))
	e3:SetCategory(CATEGORY_DESTROY+CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_MZONE)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetCountLimit(1)
	e3:SetTarget(c83755611.destg)
	e3:SetOperation(c83755611.desop)
	c:RegisterEffect(e3)
end
function c83755611.matcheck(e,c)
	local ct=c:GetMaterial():Filter(Card.IsRace,nil,RACE_WYRM):GetClassCount(Card.GetOriginalAttribute)
	e:SetLabel(ct)
end
function c83755611.tdcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_SYNCHRO
end
function c83755611.tdtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsAbleToDeck() end
	local ct=e:GetLabelObject():GetLabel()
	if chk==0 then return Duel.IsExistingTarget(Card.IsAbleToDeck,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil) and ct>0 end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,Card.IsAbleToDeck,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,ct,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,g:GetCount(),0,0)
end
function c83755611.tdop(e,tp,eg,ep,ev,re,r,rp)
	local tg=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local g=tg:Filter(Card.IsRelateToEffect,nil,e)
	if g:GetCount()>0 then
		Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
	end
end
function c83755611.filter(c,e,tp)
	return c:IsLevelBelow(4) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c83755611.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if chk==0 then
		if not Duel.IsExistingTarget(c83755611.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp) then return false end
		if ft<0 then
			return false
		elseif ft>0 then
			return Duel.IsExistingTarget(Card.IsDestructable,tp,LOCATION_ONFIELD,0,1,nil)
		else
			return Duel.IsExistingTarget(Card.IsDestructable,tp,LOCATION_MZONE,0,1,nil)
		end
	end
	local g1=nil
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	if ft>0 then
		g1=Duel.SelectTarget(tp,Card.IsDestructable,tp,LOCATION_ONFIELD,0,1,1,nil)
	else
		g1=Duel.SelectTarget(tp,Card.IsDestructable,tp,LOCATION_MZONE,0,1,1,nil)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g2=Duel.SelectTarget(tp,c83755611.filter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g1,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g2,1,0,0)
end
function c83755611.desop(e,tp,eg,ep,ev,re,r,rp)
	local ex,g1=Duel.GetOperationInfo(0,CATEGORY_DESTROY)
	local ex,g2=Duel.GetOperationInfo(0,CATEGORY_SPECIAL_SUMMON)
	local tc1=g1:GetFirst()
	local tc2=g2:GetFirst()
	if tc1:IsRelateToEffect(e) and tc2:IsRelateToEffect(e) and Duel.Destroy(tc1,REASON_EFFECT)~=0 then
		Duel.SpecialSummon(tc2,0,tp,tp,false,false,POS_FACEUP)
	end
end
