--ゴヨウ·ガーディアン
function c7391448.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,nil,aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(7391448,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCondition(c7391448.spcon)
	e1:SetTarget(c7391448.sptg)
	e1:SetOperation(c7391448.spop)
	c:RegisterEffect(e1)
end
function c7391448.spcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local t=Duel.GetAttackTarget()
	if ev==1 then t=Duel.GetAttacker() end
	if not c:IsRelateToBattle() or c:IsFacedown() then return false end
	e:SetLabelObject(t)
	return t:GetLocation()==LOCATION_GRAVE and t:IsType(TYPE_MONSTER)
end
function c7391448.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	local t=e:GetLabelObject()
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and t:IsCanBeSpecialSummoned(e,0,tp,false,false) end
	t:CreateEffectRelation(e)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,t,1,0,LOCATION_GRAVE)
end
function c7391448.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=e:GetLabelObject()
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
	end
end
