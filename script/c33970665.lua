--ポンコツの意地
function c33970665.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c33970665.target)
	e1:SetOperation(c33970665.activate)
	c:RegisterEffect(e1)
end
function c33970665.filter(c,e,tp)
	return c:IsSetCard(0x24) and (c:IsCanBeSpecialSummoned(e,0,tp,false,false)
		or c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEUP,1-tp))
end
function c33970665.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c33970665.filter(chkc,e,tp) end
	if chk==0 then return (Duel.GetLocationCount(tp,LOCATION_MZONE)>0 or Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0)
		and Duel.IsExistingTarget(c33970665.filter,tp,LOCATION_GRAVE,0,3,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(33970665,2))
	local g=Duel.SelectTarget(tp,c33970665.filter,tp,LOCATION_GRAVE,0,3,3,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c33970665.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	local ft1=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local ft2=Duel.GetLocationCount(1-tp,LOCATION_MZONE)
	if g:GetCount()==0 or (ft1==0 and ft2==0) then return end
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_SPSUMMON)
	local tc=g:Select(1-tp,1,1,nil):GetFirst()
	local s1=ft1>0 and tc:IsCanBeSpecialSummoned(e,0,tp,false,false)
	local s2=ft2>0 and tc:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEUP,1-tp)
	if s1 and s2 then op=Duel.SelectOption(tp,aux.Stringid(33970665,0),aux.Stringid(33970665,1))
	elseif s1 then op=Duel.SelectOption(tp,aux.Stringid(33970665,0))
	elseif s2 then op=Duel.SelectOption(tp,aux.Stringid(33970665,1))+1
	else op=2 end
	if op==0 then Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	elseif op==1 then Duel.SpecialSummon(tc,0,tp,1-tp,false,false,POS_FACEUP) end
	g:RemoveCard(tc)
	Duel.Remove(g,POS_FACEUP,REASON_EFFECT)
end
