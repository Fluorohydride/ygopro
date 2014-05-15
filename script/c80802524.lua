--고스트릭 브레이크
function c80802524.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)	
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c80802524.condition)
	e1:SetTarget(c80802524.target)
	e1:SetOperation(c80802524.activate)
	c:RegisterEffect(e1)
end
function c54149433.cfilter(c,tp)
	return c:IsPreviousLocation(LOCATION_MZONE) and c:GetPreviousControler()==tp and c:IsSetCard(0x8d)
end
function c54149433.spcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:GetCount()==1 and eg:IsExists(c54149433.cfilter,1,nil,tp) and rp~=tp
end
function c80802524.filter(c,e,tp,code)
	return c:IsSetCard(0x8d) and c:GetCode()~=code and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c80802524.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local code=eg:GetFirst():GetCode()
	if chkc then return chkc:GetControler()==tp and chkc:GetLocation()==LOCATION_GRAVE and c80802524.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>1
		and Duel.IsExistingTarget(c80802524.filter,tp,LOCATION_GRAVE,0,2,nil,e,tp,code) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c80802524.filter,tp,LOCATION_GRAVE,0,2,2,nil,e,tp,code)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,2,0,0)
end
function c80802524.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg0=g:Filter(Card.IsRelateToEffect,nil,e)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if sg0:GetCount()==0 or ft<=0 then return end
	if ft<sg0:GetCount() then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		sg=sg0:FilterSelect(tp,c21007444.filter,ft,ft,nil,e,tp)
	else
		sg=sg0:Clone()
	end
	if sg:GetCount()>0 then
		Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEDOWN_DEFENCE)
		Duel.ConfirmCards(1-tp,g)
	end
end