--ファイヤー・ハンド
function c68535320.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(68535320,0))
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c68535320.condition)
	e1:SetTarget(c68535320.target)
	e1:SetOperation(c68535320.operation)
	c:RegisterEffect(e1)
end
function c68535320.condition(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:GetPreviousControler()==tp and c:IsReason(REASON_DESTROY+REASON_BATTLE) and rp~=tp
end
function c68535320.dfilter(c)
	return c:IsType(TYPE_MONSTER) and c:IsDestructable()
end
function c68535320.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsControler(1-tp) and c68535320.dfilter(chkc) end
	if chk==0 then return 
		Duel.IsExistingTarget(c68535320.dfilter,tp,0,LOCATION_ONFIELD,1,nil)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c68535320.dfilter,tp,0,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end

function c68535320.spfilter(c)
	return c:GetCode()==95929069 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c68535320.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		local g=Duel.GetMatchingGroup(c68535320.spfilter,tp,LOCATION_DECK,0,nil,e,tp)
		if Duel.Destroy(tc,REASON_EFFECT)
		and Duel.SelectYesNo(tp,aux.Stringid(68535320,1))
		and g:GetCount()>0
		then
			g:Select(tp,1,1,nil)
			Duel.BreakEffect()
			Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end