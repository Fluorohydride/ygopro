--キメラテック·フォートレス·ドラゴン
function c79229522.initial_effect(c)
	c:EnableReviveLimit()
	--spsummon condition
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(c79229522.splimit)
	c:RegisterEffect(e1)
	--special summon rule
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SPSUMMON_PROC)
	e2:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e2:SetRange(LOCATION_EXTRA)
	e2:SetCondition(c79229522.sprcon)
	e2:SetOperation(c79229522.sprop)
	c:RegisterEffect(e2)
	--cannot be fusion material
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e3:SetCode(EFFECT_CANNOT_BE_FUSION_MATERIAL)
	e3:SetValue(1)
	c:RegisterEffect(e3)
end
c79229522.material_count=1
c79229522.material={70095154}
function c79229522.splimit(e,se,sp,st)
	return e:GetHandler():GetLocation()~=LOCATION_EXTRA
end
function c79229522.spfilter1(c,tp)
	return c:IsCode(70095154) and c:IsAbleToGraveAsCost() and (c:IsControler(tp) or c:IsFaceup())
		and Duel.IsExistingMatchingCard(c79229522.spfilter2,tp,LOCATION_MZONE,LOCATION_MZONE,1,c,tp)
end
function c79229522.spfilter2(c,tp)
	return c:IsRace(RACE_MACHINE) and c:IsAbleToGraveAsCost() and c:IsCanBeFusionMaterial() and (c:IsControler(tp) or c:IsFaceup())
end
function c79229522.sprcon(e,c)
	if c==nil then return true end 
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c79229522.spfilter1,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil,tp)
end
function c79229522.sprop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(79205581,0))
	local g1=Duel.SelectMatchingCard(tp,c79229522.spfilter1,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,nil,tp)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(79205581,1))
	local g2=Duel.SelectMatchingCard(tp,c79229522.spfilter2,tp,LOCATION_MZONE,LOCATION_MZONE,1,10,g1:GetFirst(),tp)
	g1:Merge(g2)
	Duel.SendtoGrave(g1,REASON_COST)
	--spsummon condition
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SET_BASE_ATTACK)
	e1:SetReset(RESET_EVENT+0xff0000)
	e1:SetValue(g1:GetCount()*1000)
	c:RegisterEffect(e1)
end
