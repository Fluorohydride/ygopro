--メンタルオーバー・デーモン
function c24221808.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,aux.FilterBoolFunction(Card.IsRace,RACE_PSYCHO),aux.NonTuner(Card.IsRace,RACE_PSYCHO),2)
	c:EnableReviveLimit()
	--remove
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(24221808,0))
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetTarget(c24221808.rmtg)
	e1:SetOperation(c24221808.rmop)
	c:RegisterEffect(e1)
	local sg=Group.CreateGroup()
	sg:KeepAlive()
	e1:SetLabelObject(sg)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(24221808,1))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetCondition(c24221808.spcon)
	e2:SetTarget(c24221808.sptg)
	e2:SetOperation(c24221808.spop)
	e2:SetLabelObject(sg)
	c:RegisterEffect(e2)
end
function c24221808.rmfilter(c)
	return c:IsRace(RACE_PSYCHO) and c:IsAbleToRemove()
end
function c24221808.rmtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c24221808.rmfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c24221808.rmfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c24221808.rmfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
end
function c24221808.rmop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)
		if c:IsRelateToEffect(e) then
			local sg=e:GetLabelObject()
			if c:GetFlagEffect(24221808)==0 then
				sg:Clear()
				c:RegisterFlagEffect(24221808,RESET_EVENT+0x1680000,0,1)
			end
			sg:AddCard(tc)
			tc:CreateRelation(c,RESET_EVENT+0x1fe0000)
		end
	end
end
function c24221808.spcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD) and e:GetHandler():GetFlagEffect(24221808)~=0
end
function c24221808.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_REMOVED)
end
function c24221808.spfilter(c,rc,e,tp)
	return c:IsRelateToCard(rc) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c24221808.spop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local sg=g:FilterSelect(tp,c24221808.spfilter,ft,ft,nil,e:GetHandler(),e,tp)
	Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
end
