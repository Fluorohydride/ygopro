--インフェルニティ・フォース
function c18712704.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_BE_BATTLE_TARGET)
	e1:SetCondition(c18712704.condition)
	e1:SetTarget(c18712704.target)
	e1:SetOperation(c18712704.activate)
	c:RegisterEffect(e1)
end
function c18712704.condition(e,tp,eg,ep,ev,re,r,rp)
	local att=eg:GetFirst()
	return Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)==0 and att:IsFaceup() and att:IsSetCard(0xb)
end
function c18712704.filter(c,e,tp)
	return c:IsSetCard(0xb) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c18712704.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local tg=Duel.GetAttacker()
	if chkc then return false end
	if chk==0 then return tg:IsOnField() and tg:IsDestructable() and tg:IsCanBeEffectTarget(e) and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c18712704.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.SetTargetCard(tg)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c18712704.filter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,tg,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c18712704.activate(e,tp,eg,ep,ev,re,r,rp)
	local ex,sg=Duel.GetOperationInfo(0,CATEGORY_SPECIAL_SUMMON)
	local ex,dg=Duel.GetOperationInfo(0,CATEGORY_DESTROY)
	local sc=sg:GetFirst()
	local dc=dg:GetFirst()
	if dc:IsRelateToEffect(e) and dc:IsAttackPos() then
		Duel.Destroy(dg,REASON_EFFECT)
		if sc:IsRelateToEffect(e) and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
			and sc:IsCanBeSpecialSummoned(e,0,tp,false,false) then
			Duel.BreakEffect()
			Duel.SpecialSummon(sc,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end
